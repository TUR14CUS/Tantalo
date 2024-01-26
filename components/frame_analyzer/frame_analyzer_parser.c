#include "frame_analyzer_parser.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "arpa/inet.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include "esp_wifi_types.h"

#include "frame_analyzer_types.h"

static const char *TAG = "frame_analyzer:parser";

ESP_EVENT_DEFINE_BASE(FRAME_ANALYZER_EVENTS);

/**
 * @brief Debug function to print raw frame to serial
 * 
 * @param frame The WiFi promiscuous packet
 */
void print_raw_frame(const wifi_promiscuous_pkt_t *frame){
    for(unsigned i = 0; i < frame->rx_ctrl.sig_len; i++) {
        printf("%02x", frame->payload[i]);
    }
    printf("\n");
}

/**
 * @brief Debug functions to print MAC address from given buffer to serial
 * 
 * @param a The MAC address buffer
 */
void print_mac_address(const uint8_t *a){
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
    a[0], a[1], a[2], a[3], a[4], a[5]);
    printf("\n");
}

/**
 * @brief Checks if the BSSID of the frame matches the given BSSID
 * 
 * @param frame The WiFi promiscuous packet
 * @param bssid The BSSID to compare against
 * @return true if the BSSID matches, false otherwise
 */
bool is_frame_bssid_matching(wifi_promiscuous_pkt_t *frame, uint8_t *bssid) {
    data_frame_mac_header_t *mac_header = (data_frame_mac_header_t *) frame->payload;
    return memcmp(mac_header->addr3, bssid, 6) == 0;
}

/**
 * @brief Parses an EAPoL packet from a data frame
 * 
 * @param frame The data frame
 * @return The parsed EAPoL packet, or NULL if not an EAPoL packet
 */
eapol_packet_t *parse_eapol_packet(data_frame_t *frame) {
    uint8_t *frame_buffer = frame->body;

    if(frame->mac_header.frame_control.protected_frame == 1) {
        ESP_LOGV(TAG, "Protected frame, skipping...");
        return NULL;
    }

    if(frame->mac_header.frame_control.subtype > 7) {
        ESP_LOGV(TAG, "QoS data frame");
        // Skipping QoS field (2 bytes)
        frame_buffer += 2;
    }

    // Skipping LLC SNAP header (6 bytes)
    frame_buffer += sizeof(llc_snap_header_t);

    // Check if frame is type of EAPoL
    if(ntohs(*(uint16_t *) frame_buffer) == ETHER_TYPE_EAPOL) {
        ESP_LOGD(TAG, "EAPOL packet");
        frame_buffer += 2;
        return (eapol_packet_t *) frame_buffer; 
    }
    return NULL;
}

/**
 * @brief Parses an EAPoL-Key packet from an EAPoL packet
 * 
 * @param eapol_packet The EAPoL packet
 * @return The parsed EAPoL-Key packet, or NULL if not an EAPoL-Key packet
 */
eapol_key_packet_t *parse_eapol_key_packet(eapol_packet_t *eapol_packet){
    if(eapol_packet->header.packet_type != EAPOL_KEY){
        ESP_LOGD(TAG, "Not an EAPoL-Key packet.");
        return NULL;
    }
    return (eapol_key_packet_t *) eapol_packet->packet_body;
}

/**
 * @brief Parses all PMKIDs to linked list structure 
 * 
 * It crawls through key data buffer and looks for PMKIDs.
 * If a PMKID element is found, it is saved into the list of PMKIDs.
 * @param key_data The key data buffer
 * @param length The length of the key data buffer
 * @return The head of the PMKID linked list
 */
static pmkid_item_t *parse_pmkid_from_key_data(uint8_t *key_data, const uint16_t length){
    uint8_t *key_data_index = key_data;
    uint8_t *key_data_max_index = key_data + length;

    pmkid_item_t *pmkid_item_head = NULL;
    key_data_field_t *key_data_field;
    do{
        key_data_field = (key_data_field_t *) key_data_index;

        ESP_LOGV(TAG, "EAPOL-Key -> Key-Data -> type=%x; length=%x; oui=%x; data_type=%x",
                    key_data_field->type, 
                    key_data_field->length, 
                    key_data_field->oui,
                    key_data_field->data_type);
        
        if(key_data_field->type != KEY_DATA_TYPE){
            ESP_LOGD(TAG, "Wrong type %x (expected %x)", key_data_field->type, KEY_DATA_TYPE);
            continue;
        }

        if(ntohl(key_data_field->oui) != KEY_DATA_OUI_IEEE80211){
            ESP_LOGD(TAG, "Wrong OUI %x (expected %x)", key_data_field->oui, KEY_DATA_OUI_IEEE80211);
            continue;
        }

        if(key_data_field->data_type != KEY_DATA_DATA_TYPE_PMKID_KDE){
            ESP_LOGD(TAG, "Wrong data type %x (expected %x)", key_data_field->data_type, KEY_DATA_DATA_TYPE_PMKID_KDE);
            continue;
        }

        ESP_LOGI(TAG, "Found PMKID: ");
        pmkid_item_t *pmkid_item = (pmkid_item_t *) malloc(sizeof(pmkid_item_t));
        pmkid_item->next = pmkid_item_head;
        pmkid_item_head = pmkid_item;
        memcpy(pmkid_item->pmkid, key_data_field->data, 16);
        for(unsigned i = 0; i < 16; i++){
            printf("%02x", pmkid_item->pmkid[i]);
        }
        printf("\n");

    } while((key_data_index = key_data_field->data + key_data_field->length - 4 + 1) < key_data_max_index); 

    return pmkid_item_head;
}

/**
 * @brief Parses PMKID from an EAPoL-Key packet
 * 
 * @param eapol_key The EAPoL-Key packet
 * @return The head of the PMKID linked list
 */
pmkid_item_t *parse_pmkid(eapol_key_packet_t *eapol_key){
    if(eapol_key->key_data_length == 0){
        ESP_LOGD(TAG, "Empty Key Data");
        return NULL;
    }

    if(eapol_key->key_information.encrypted_key_data == 1){
        ESP_LOGD(TAG, "Key Data encrypted");
        return NULL;
    }

    return parse_pmkid_from_key_data(eapol_key->key_data, ntohs(eapol_key->key_data_length));
}