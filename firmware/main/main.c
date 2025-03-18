#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_server.h>
#include "esp_log.h"
#include "wifi_api.h"
#include "http_server.h"
#include "io_config.h"
#include "i2c_master.h"
#include "esp_sleep.h" 

// Private config should include the defenitions:
// WIFI_SSID
// WIFI_PASSWORD
#include "private_config.h" 
#define TAG "main"

// Wake up count in RTC memory
RTC_DATA_ATTR static int wake_count = 0;

// FreeRTOS tasks definitions:
void test_task(void* pvParameters){
    printf("Running test task on wakeup %d\n", wake_count);
    vTaskDelete(NULL);
}

void app_main(void) {
    wake_count++;
    ESP_LOGI(TAG, "Wake count: %d", wake_count);
    
    gpio_config(&io_config); 
    init_wifi();
    
    // Init wifi connection
    esp_err_t ret = connect_wifi(WIFI_SSID, WIFI_PASSWORD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        return;
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
        return;
    }
    
    ESP_LOGI(TAG, "Performing tasks before sleep");
    xTaskCreate(test_task, "test_task", 2048, NULL, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(10000));

    ESP_LOGI(TAG, "Cleaning up before sleep");
    stop_webserver(server_handle);
    deinit_wifi();
    
    ESP_LOGI(TAG, "Entering deep sleep for 10 seconds");
    esp_sleep_enable_timer_wakeup(10 * 1000000); // 10 seconds in microseconds
    
    esp_deep_sleep_start();
    
}

