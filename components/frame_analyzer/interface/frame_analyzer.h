#ifndef FRAME_ANALYZER_H
#define FRAME_ANALYZER_H

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(FRAME_ANALYZER_EVENTS);

typedef enum {
    DATA_FRAME_EVENT_EAPOLKEY_FRAME,
    DATA_FRAME_EVENT_PMKID
} frame_analyzer_event_t;

/**
 * @brief Types of information requested for frame analysis.
 */
typedef enum {
    SEARCH_EAPOL_HANDSHAKE,
    SEARCH_PMKID
} search_type_t;

/**
 * @brief Starts capturing frames for analysis.
 *
 * This function starts capturing frames for analysis based on the specified search type and BSSID.
 *
 * @param search_type The type of frames to search for.
 * @param bssid The BSSID (MAC address) to filter frames by.
 */
void frame_analyzer_capture_start(search_type_t search_type, const uint8_t *bssid);

/**
 * @brief Stops frame analysis.
 */
void frame_analyzer_capture_stop();

#endif
