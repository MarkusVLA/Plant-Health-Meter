#pragma once

#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_err.h>

#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>

esp_err_t init_wifi(void);

esp_err_t deinit_wifi(void);

esp_err_t connect_wifi(char *wifi_ssid, char *password);
    
esp_err_t disconnect_wifi(void);
