#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_server.h>
#include <stdint.h>
#include "esp_log.h"
#include "wifi_api.h"
#include "http_server.h"
#include "firebase_api.h"
#include "io_config.h"
#include "i2c_master.h"
#include "esp_sleep.h"
#include "time_sync.h"
#include "sensor_api.h"
#include "packet.h"
#include "ble_wifi_credentials.h"
#include "data_store.h"
#include "BME280.h"
#include "ADS1115.h"

// Private config should include the defenitions:
// FIREBASE_HOST "xxx-rtdb.europe-west1.firebasedatabase.app"
// FIREBASE_AUTH "" // TODO: set-up firebase auth
#include "private_config.h" 
#define TAG "main"

// Wake up count in RTC memory
RTC_DATA_ATTR static int wake_count = 0;

void app_main(void) {
    wake_count++;
    ESP_LOGI(TAG, "Wake count: %d", wake_count);

    gpio_config(&io_config); 

    // Enable boost converter
    gpio_set_level(BOOST_EN, 1);
    init_wifi();

    char ssid[64] = {0};
    char password[64] = {0};

    // Try loading Wi-Fi credentials from NVS
    esp_err_t creds_loaded = load_wifi_credentials(ssid, sizeof(ssid), password, sizeof(password));
    esp_err_t ret = ESP_FAIL;

    if (creds_loaded == ESP_OK) {
        ESP_LOGI(TAG, "Loaded credentials from NVS: SSID=%s", ssid);
        ret = connect_wifi(ssid, password);
    } else {
        ESP_LOGW(TAG, "No Wi-Fi credentials found in NVS");
    }

    // If credentials failed to load or Wi-Fi connection failed, go into BLE pairing mode
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Wi-Fi connection failed, entering BLE mode");
        ble_wifi_start_server();
        return;
    }

    // Successfully connected
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA"), &ip_info);
    ESP_LOGI(TAG, "Server running on address: " IPSTR, IP2STR(&ip_info.ip));

    // Set the transmit power
    esp_err_t power_ret = esp_wifi_set_max_tx_power(55); // 8 - 84
    if (power_ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set Wi-Fi transmit power: %s", esp_err_to_name(power_ret));
    }

    // Init REST API
    httpd_handle_t server_handle = start_webserver();

    // Init I2C driver
    ret = begin_i2c();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init I2C driver");
        return;
    }

    // Sync time over NTP
    sync_time();

    // Fetch sensor packet and send to Firebase
    sensor_data_packet_t data_packet = get_sensor_data_packet();

    // Convert data to JSON
    char encoded_data[JSON_PACKET_LEN];
    packet_to_json(encoded_data, JSON_PACKET_LEN, &data_packet);

    // Send data to Firebase
    send_to_firebase(encoded_data);

    // Init and test BME280 / ADS1115
    init_sensors();
    ads1115_dump_config();
    uint16_t temp_sensor_val = 0;
    float adc_val = 0;

    for (int i = 0; i < 10; i++) {
        bme280_read_temperature(&temp_sensor_val);
        adc_val = read_ain1();
        ESP_LOGI(TAG, "temperature: %d", temp_sensor_val);
        ESP_LOGI(TAG, "ADC: %.2f", adc_val);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Clean up and sleep
    ESP_LOGI(TAG, "Cleaning up before sleep");
    stop_webserver(server_handle);
    deinit_wifi();

    ESP_LOGI(TAG, "Entering deep sleep");
    esp_sleep_enable_timer_wakeup(10 * 1000000); // 10 seconds
    esp_deep_sleep_start();
}
