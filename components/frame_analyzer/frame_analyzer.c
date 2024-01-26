/**
 * @file frame_analyzer.c
 * @brief Implementation of the frame analyzer module.
 * 
 * This module provides functions for analyzing data frames captured by a sniffer.
 * It includes functions for starting and stopping the frame capture, as well as
 * handling different types of data frames.
 */

#include "frame_analyzer.h"

#include <stdint.h>
#include <string.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"

#include "wifi_controller.h"
#include "frame_analyzer_parser.h"

static const char *TAG = "frame_analyzer";
static uint8_t target_bssid[6];
static search_type_t search_type = -1;

/**
 * @brief Handler function for data frames.
 * 
 * This function is called when a data frame is captured by the sniffer.
 * It analyzes the frame and performs specific actions based on the search type.
 * 
 * @param args Unused argument.
 * @param event_base The event base.
 * @param event_id The event ID.
 * @param event_data The event data, which contains the captured frame.
 */
static void data_frame_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGV(TAG, "Handling DATA frame");
    wifi_promiscuous_pkt_t *frame = (wifi_promiscuous_pkt_t *) event_data;

    if (!is_frame_bssid_matching(frame, target_bssid)) {
        ESP_LOGV(TAG, "Not matching BSSIDs.");
        return;
    }

    eapol_packet_t *eapol_packet = parse_eapol_packet((data_frame_t *) frame->payload);
    if (eapol_packet == NULL) {
        ESP_LOGV(TAG, "Not an EAPOL packet.");
        return;
    }

    eapol_key_packet_t *eapol_key_packet = parse_eapol_key_packet(eapol_packet);
    if (eapol_key_packet == NULL) {
        ESP_LOGV(TAG, "Not an EAPOL-Key packet");
        return;
    }

    if (search_type == SEARCH_HANDSHAKE) {
        // TODO handle timeouts properly by e.g. using a for loop
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_post(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_EAPOLKEY_FRAME, frame, sizeof(wifi_promiscuous_pkt_t) + frame->rx_ctrl.sig_len, portMAX_DELAY));
        return;
    }

    if (search_type == SEARCH_PMKID) {
        pmkid_item_t *pmkid_items = parse_pmkid(eapol_key_packet);
        if (pmkid_items == NULL) {
            return;
        }
        ESP_ERROR_CHECK(esp_event_post(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_PMKID, &pmkid_items, sizeof(pmkid_item_t *), portMAX_DELAY));
        return;
    }
}

/**
 * @brief Start capturing frames for analysis.
 * 
 * This function starts capturing frames for analysis based on the specified search type and BSSID.
 * 
 * @param search_type_arg The search type for frame analysis.
 * @param bssid The BSSID to filter captured frames.
 */
void frame_analyzer_capture_start(const frame_analyzer_search_type_t *search_type_and_bssid) {
    search_type = search_type_and_bssid->search_type;
    memcpy(target_bssid, search_type_and_bssid->bssid, 6);
    ESP_ERROR_CHECK(esp_event_handler_register(SNIFFER_EVENTS, SNIFFER_EVENT_CAPTURED_DATA, &data_frame_handler, NULL));
}

/**
 * @brief Stop capturing frames for analysis.
 * 
 * This function stops capturing frames for analysis.
 */
void frame_analyzer_capture_stop() {
    ESP_ERROR_CHECK(esp_event_handler_unregister(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, &data_frame_handler));
}
