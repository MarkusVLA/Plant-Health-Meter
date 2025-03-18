// Simple starting point program connects to wifi and creates a HTTP server.

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_server.h>

#include "esp_log.h"
#include "wifi_api.h"
#include "rest_api.h"
#include "io_config.h"
#include "i2c_master.h"

// Private config should include the defenitions:
// WIFI_SSID
// WIFI_PASSWORD
#include "private_config.h" 

#define TAG "main"

void app_main(void) {
    // IO configuration setup
    gpio_config(&io_config); 
    init_wifi();
    
    // Init wifi connection
    esp_err_t ret = connect_wifi(WIFI_SSID, WIFI_PASSWORD);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        ESP_LOGW(TAG, "Continuing to main without WiFi!");
    }

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA"), &ip_info);
    ESP_LOGI(TAG, "Server running on address: " IPSTR, IP2STR(&ip_info.ip));

    // Init REST API
    httpd_handle_t server_handle = start_webserver();

    // Init i2c driver:
    ret = begin_i2c();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init i2c driver");
    }

    int running = 1;
    while(running) {
        vTaskDelay(pdMS_TO_TICKS(20));
        test_i2c();
    }

    stop_webserver(server_handle);
    deinit_wifi();
}

