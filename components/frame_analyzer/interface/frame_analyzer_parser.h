#ifndef FRAME_ANALYZER_PARSER_H
#define FRAME_ANALYZER_PARSER_H

#include <stdint.h>
#include "esp_wifi_types.h"

#include "frame_analyzer_types.h"

/**
 * @brief Checks if the BSSID of a given frame matches a specified BSSID.
 *
 * @param frame Pointer to the WiFi frame to be checked.
 * @param bssid Pointer to the BSSID to compare against.
 * @return true if the BSSIDs match, false otherwise.
 */
bool is_frame_bssid_matching(wifi_promiscuous_pkt_t *wifi_frame, uint8_t *target_bssid);

/**
 * @brief Parses EAPoL packet from given data frame.
 * 
 * @param data_frame 
 * @return Pointer to the parsed EAPoL packet if successful, NULL otherwise.
 */
eapol_packet_t *parse_eapol_packet(data_frame_t *data_frame);

/**
 * @brief Parses EAPoL-Key packet from EAPoL packet.
 * 
 * @note The result does not include the EAPoL header.
 * @param eapol_packet 
 * @return Pointer to the parsed EAPoL-Key packet if successful, NULL otherwise.
 */
eapol_key_packet_t *parse_eapol_key_packet(eapol_packet_t *eapol_packet);

/**
 * @brief Parses PMKIDs from EAPoL-Key packet.
 * 
 * @param eapol_key 
 * @return Pointer to the linked list of PMKIDs if successful, NULL otherwise.
 */
pmkid_item_t *parse_pmkid(eapol_key_packet_t *eapol_key);

#endif
