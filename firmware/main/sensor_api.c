#include "sensor_api.h"
#include "esp_log.h"
#include "BME280.h"
#include "ADS1115.h"
#include <stdint.h>
#include "driver/ledc.h"

#define TAG "Sensor API"

static const ledc_timer_config_t pwm_timer_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_2_BIT, 
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 350e3,                      
    .clk_cfg = LEDC_AUTO_CLK,
};

// GPIO4
static const ledc_channel_config_t moisture_sensor_channel = {
    .gpio_num = GPIO_NUM_4,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .duty = 2,
    .hpoint = 0
};

// Has to be set to 1 before sensor data can be called
static int sensor_init_status = 0;

esp_err_t init_sensors(void) {
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initalizing PWM timer");
    ret = ledc_timer_config(&pwm_timer_config);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initalize PWM driver, moisture sensor will not be available.");
    }

    ret = ledc_channel_config(&moisture_sensor_channel);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initalize moisture sensor input channel, moisture sensor will not be available.");
    }
    
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
        return 0x00;
    }
    uint16_t raw_temp_val = 0;
    bme280_read_temperature(&raw_temp_val);
    return (float) raw_temp_val; // TODO. Proper conversion
}

static float get_moisture(void) {
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x00;
    }
    // Create input signal for moisture sensor.
    ledc_set_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel, 2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel);
    vTaskDelay(pdMS_TO_TICKS(MOISTURE_SENSOR_SETTLE_TIME_MS));
    float result = read_ain0(); // Raw analog voltage.
    // Turn off PWM signal after conversion,
    ledc_set_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel);
    return result;
}

static float get_light_level() {
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x00;
    }
    return read_ain1();
}

sensor_data_packet_t get_sensor_data_packet(void){
    // Example packet, other values added later.
    sensor_data_packet_t ret = {
        .humidity = 0x00,
        .temperature = get_temperature(),
        .light = get_light_level(),
        .moisture = get_moisture(),
        .header = 0x00
    };
    
    // copy of generated packet
    return ret;
} 

