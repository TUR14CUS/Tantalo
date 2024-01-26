/**
 * @file ap_scanner.h
 * @brief Provides an interface for AP scanning functionality.
 */

#ifndef AP_SCANNER_H
#define AP_SCANNER_H

#include "esp_wifi_types.h"

/**
 * @brief Linked list of wifi_ap_record_t records.
 * 
 */
typedef struct {
    uint16_t count;                  /**< Number of records in the list */
    wifi_ap_record_t records[CONFIG_SCAN_MAX_AP];  /**< Array of wifi_ap_record_t records */
} wifictl_ap_records_t;

/**
 * @brief Scans for nearby access points and stores the results in a linked list.
 * 
 */
void wifictl_scan_nearby_aps();

/**
 * @brief Returns a pointer to the current list of scanned access points.
 * 
 * @return const wifictl_ap_records_t* 
 */
const wifictl_ap_records_t *wifictl_get_ap_records();

/**
 * @brief Returns a pointer to the wifi_ap_record_t record at the given index.
 *
 * @param index Zero-based index of the desired record.
 * @return const wifi_ap_record_t* 
 */
const wifi_ap_record_t *wifictl_get_ap_record(unsigned index);

#endif /* AP_SCANNER_H */