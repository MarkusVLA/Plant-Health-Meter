#pragma once

#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_err.h>

#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>

// Initalize the wifi module on the esp
esp_err_t init_wifi(void);

// Deinialize the wifi module on the esp
esp_err_t deinit_wifi(void);

// Connect to a wifi network with ssid and password
esp_err_t connect_wifi(char *wifi_ssid, char *password);

// Disconnect from wifi network
esp_err_t disconnect_wifi(void);
