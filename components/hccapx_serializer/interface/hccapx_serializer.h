#ifndef HCCAPX_SERIALIZER_H
#define HCCAPX_SERIALIZER_H

#include <stdint.h>

#include "frame_analyzer_types.h"

/**
 * @brief HCCAPX structure according to reference
 * 
 * @see Ref: https://hashcat.net/wiki/doku.php?id=hccapx
 */
typedef struct __attribute__((__packed__)){
    uint32_t signature;     // HCCAPX file signature
    uint32_t version;       // HCCAPX file version
    uint8_t message_pair;   // Message pair
    uint8_t essid_len;      // Length of ESSID
    uint8_t essid[32];      // ESSID (up to 32 bytes)
    uint8_t keyver;         // Key version
    uint8_t keymic[16];     // Key MIC
    uint8_t mac_ap[6];      // MAC address of the AP
    uint8_t nonce_ap[32];   // Nonce of the AP
    uint8_t mac_sta[6];     // MAC address of the STA
    uint8_t nonce_sta[32];  // Nonce of the STA
    uint16_t eapol_len;     // Length of EAPOL data
    uint8_t eapol[256];     // EAPOL data (up to 256 bytes)
} hccapx_t;

/**
 * @brief Initializes the HCCAPX serializer with a given SSID.
 *
 * This function initializes the HCCAPX serializer with a given SSID. It clears any previous HCCAPX data.
 * If you want to save the previous HCCAPX data, first call `hccapx_serializer_get()` and copy the buffer somewhere else.
 *
 * @param ssid SSID of the AP from which the handshake frames will be coming.
 * @param size Length of the SSID string (including \0).
 */
void hccapx_serializer_init(const uint8_t *ssid, unsigned size);

/**
 * @brief Returns a pointer to the HCCAPX formatted binary data buffer.
 *
 * This function returns a pointer to the HCCAPX formatted binary data buffer.
 *
 * @return hccapx_t* 
 */
hccapx_t *hccapx_serializer_get();

/**
 * @brief Adds new handshake frames into the current HCCAPX.
 *
 * This function processes the given frames and extracts the relevant data.
 * If the frame contains a handshake from a different STA than the one that was already added before,
 * the frame will be skipped and an error message will be printed.
 *
 * @param frame Data frame with EAPoL-Key packet.
 */
void hccapx_serializer_add_frame(data_frame_t *frame);

#endif