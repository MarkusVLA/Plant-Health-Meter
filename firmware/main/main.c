#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include "lwip/sockets.h"
#include "esp_log.h"

#include "wifi_api.h"

#define WIFI_SSID "DNA-WIFI-42A8"
#define WIFI_PASSWORD "80849779480"
#define SERVER_IP "192.168.1.101"  
#define SERVER_PORT 8080
#define TRANSMIT_DELAY_MS 6000

#define TAG "tcp_client"

void send_sensor_data(float sensor_value) {
    struct sockaddr_in server_addr;
    char data_buffer[32];


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket");
        return;
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
    }

    close(sock);
}

void app_main(void) {
    init_wifi();
    esp_err_t ret = connect_wifi(WIFI_SSID, WIFI_PASSWORD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        return;
    }

    int running = 1;
    while(running) {
        float sensor_reading = 6.9; 
        send_sensor_data(sensor_reading);
        vTaskDelay(pdMS_TO_TICKS(TRANSMIT_DELAY_MS)); 
    }
}

