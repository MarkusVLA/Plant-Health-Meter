#include "sensor_api.h"
#include "esp_log.h"
#include "BME280.h"
#include "ADS1115.h"
#include <stdint.h>

#define TAG "Sensor API"

// Has to be set to 1 before sensor data can be called
static int sensor_init_status = 0;

esp_err_t init_sensors(void) {
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Initializing sensors ...");
    // Init the BME280 temperature/humidity sensor first
    ret = init_bme280();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initialize BME280");
    } else {
        ESP_LOGI(TAG, "BME280 initalized succesfully");
    }

    ret = ads1115_init();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initalize ADS1115!");
    } else {
        ESP_LOGI(TAG, "ADS1115 initialized succesfully");
    }

    // init rest of the sensors
    sensor_init_status = 1;
    return ret;

}

static float get_temperature(void){
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x0000;
    }
    uint16_t raw_temp_val = 0;
    bme280_read_temperature(&raw_temp_val);
    return (float) raw_temp_val; // TODO. Proper conversion
}

static float get_light_level() {
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x0000;
    }
    return read_ain1();
}

sensor_data_packet_t get_sensor_data_packet(void){
    // Example packet, other values added later.
    sensor_data_packet_t ret = {
        .humidity = 0x00,
        .temperature = get_temperature(),
        .light = get_light_level(),
        .moisture = 0x00,
        .header = 0x00
    };
    
    // copy of generated packet
    return ret;
} 

