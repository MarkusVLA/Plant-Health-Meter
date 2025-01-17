// Simple starting program that sends text trough the serial port and blinks a LED

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2

gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << LED_PIN), // Write to 64 bit pin register.
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

void app_main(void){
    gpio_config(&io_conf); // Load io config

    int running = true;
    while (running){
        gpio_set_level(LED_PIN, 1);
        printf("Hello from esp32\n");
        vTaskDelay(pdMS_TO_TICKS(60));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(940));
    }
}
