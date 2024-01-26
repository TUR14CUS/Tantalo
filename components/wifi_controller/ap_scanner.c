/**
 * @file ap_scanner.c
 * @brief Implements AP scanning functionality.
 */

#include "ap_scanner.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"

static const char* TAG = "wifi_controller/ap_scanner";



/**
 * @brief Callback function for Wi-Fi scan results
 *
 * @param scan_result_event_data Pointer to scan result event data
 */
static void wifi_scan_result_handler(void *scan_result_event_data, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    wifi_ap_record_t *ap_records = (wifi_ap_record_t *) event_data;
    int ap_count = (int) event_data;

    // Print the number of access points found
    ESP_LOGI(TAG, "Number of access points found: %d", ap_count);

    // Loop through the list of access points
    for (int i = 0; i < ap_count; i++) {
        // Print the information of each access point
        ESP_LOGI(TAG, "Access Point %d:", i + 1);
        ESP_LOGI(TAG, "SSID: %s", ap_records[i].ssid);
        ESP_LOGI(TAG, "RSSI: %d dBm", ap_records[i].rssi);
        ESP_LOGI(TAG, "Authmode: %d", ap_records[i].authmode);
        ESP_LOGI(TAG, "Channel: %d", ap_records[i].primary);

        // Check if the access point uses WPA3 security
        if (ap_records[i].authmode == WIFI_AUTH_WPA3_PSK) {
            ESP_LOGI(TAG, "Access Point %d uses WPA3 security", i + 1);
        }
    }
}

/**
 * @brief Starts a Wi-Fi scan for access points
 */
void ap_scanner_start() {
    // Start a Wi-Fi scan
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));

    // Register the scan result callback function
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &wifi_scan_result_handler, NULL));
}

/**
 * @brief Stops a Wi-Fi scan for access points
 */
void ap_scanner_stop() {
    // Stop a Wi-Fi scan
    ESP_ERROR_CHECK(esp_wifi_scan_stop());

    // Unregister the scan result callback function
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &wifi_scan_result_handler));
}