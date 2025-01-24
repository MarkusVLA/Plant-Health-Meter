#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include "lwip/sockets.h"
#include "esp_log.h"
#include "wifi_api.h"
#include <math.h>

// Private config should include the defenitions:
// WIFI_SSID
// WIFI_PASSWORD
// SERVER_IP
// SERVER_PORT
#include "private_config.h" 


#define TRANSMIT_DELAY_MS 3000 
#define LED_PIN GPIO_NUM_14
#define TAG "main"

float get_sensor_val(int t){
    return 32 * sin(t * 0.03);
}

esp_err_t send_sensor_data(float sensor_value) {
    struct sockaddr_in server_addr;
    char data_buffer[32];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket");
        close(sock);
        return ESP_FAIL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        snprintf(data_buffer, sizeof(data_buffer), "%.2f", sensor_value);
        send(sock, data_buffer, strlen(data_buffer), 0);
        ESP_LOGI(TAG, "Sent sensor data: %s", data_buffer);

    } else {
        ESP_LOGE(TAG, "Server connection failed");
        close(sock);
        return ESP_FAIL;
    }

    close(sock);
    return ESP_OK;
}

void transmission_status_indicator(esp_err_t status){
    gpio_set_level(LED_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(LED_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(50)); 

    if (status == ESP_OK) {
        return; 
        
    } else if (status == ESP_FAIL){
        // Blink led again
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(LED_PIN, 0);
        return;

    } else {
        // Something unexpected happended. This should not happen
        ESP_LOGE(TAG, "Unknown transmission status indicator");
        return;
    }
}

void app_main(void) {
    // IO configuration setup
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << LED_PIN), // 64 bit mask
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    }; 

    gpio_config(&io_config); // Pass gpio_config_t pointer to load the config
    init_wifi();
    esp_err_t ret = connect_wifi(WIFI_SSID, WIFI_PASSWORD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        return;
    }
    int t = 0;

    int running = 1;
    while(running) {
        float sensor_reading = get_sensor_val(t);
        esp_err_t transmission_status = send_sensor_data(sensor_reading);

        // Indicate transmission status with LED
        transmission_status_indicator(transmission_status); 
        vTaskDelay(pdMS_TO_TICKS(TRANSMIT_DELAY_MS));
        t++;
    }
}

