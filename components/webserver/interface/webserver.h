/**
 * @file webserver.h
 * 
 * @brief Provides interface to control and communicate with Webserver component
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "esp_event.h"

// Define custom event base for webserver events
ESP_EVENT_DECLARE_BASE(WEBSERVER_EVENTS);

// Enumerate webserver events
enum {
    WEBSERVER_EVENT_ATTACK_REQUEST,  //< Event triggered when an attack request is received
    WEBSERVER_EVENT_ATTACK_RESET     //< Event triggered when an attack reset request is received
};

/**
 * @brief Struct to deserialize attack request parameters 
 */
typedef struct {
    uint8_t ap_record_id;   //< ID of the chosen AP. It can be used to access ap_records array from wifi_controller - ap_scanner
    uint8_t type;           //< Chosen type of attack
    uint8_t method;         //< Chosen method of attack
    uint8_t timeout;        //< Attack timeout in seconds
} attack_request_t;

/**
 * @brief Initializes and starts the webserver 
 */
void webserver_run();

#endif