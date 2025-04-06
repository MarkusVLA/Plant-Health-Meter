#include "sensor_api.h"
#include "esp_log.h"
#include "BME280.h"
#include "ADS1115.h"
#include "driver/ledc.h"

#define TAG "Sensor API"
#define MOISTURE_SENSOR_SETTLE_TIME_MS 25

static const ledc_timer_config_t pwm_timer_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_2_BIT, 
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 350e3,                   //350kHz Square wave input for moisture sensor.
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
    float temperature = 0.0f;
    bme280_read_temperature(&temperature);
    return temperature; // TODO. Proper conversion
}

static float get_humidity(void){
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x00;
    }

    float humidity = 0.0f;
    bme280_read_humidity(&humidity);
    return humidity; // TODO. Proper conversion
}



// Get capacitance from voltage using polynomial regression.
// Trained using python script under /simulation: Based on LT-Spice model
static float voltage_to_capacitance(float voltage) {
    // Coefficients from the polynomial equation
    const float a0 = 4856.982050f;  
    const float a1 = -5808.710660f; 
    const float a2 = 2387.227805f; 
    const float a3 = -331.288972f;  
    
    float v1 = voltage;
    float v2 = voltage * voltage;
    float v3 = v2 * voltage;
    
    // capacitance using the polynomial
    float capacitance = a0 + (a1 * v1) + (a2 * v2) + (a3 * v3);
    return capacitance;
}

// Linearly interpolate cpacitance to moisture %, assuming linear relation ship.
static float capacitance_to_moisture_percent(float capacitance, float min_cap, float max_cap) {
    if (capacitance <= min_cap) return 0.0f;
    if (capacitance >= max_cap) return 100.0f;
    return (capacitance - min_cap) / (max_cap - min_cap) * 100.0f;
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
    float capacitance = voltage_to_capacitance(read_ain0()); 
    float moisture = capacitance_to_moisture_percent(capacitance, 27.0f, 300.0f);

    // Turn off PWM signal after conversion,
    ledc_set_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, moisture_sensor_channel.channel);
    return moisture;
}

static float voltage_to_light_level(float voltage) {
    const float Rf = 33000.0f;  // Feedback resistor (33k ohm)
    const float current_at_1mW_per_cm2 = 50.0e-6f;  // 50μA at 1mW/cm²
    float photodiode_current = voltage / Rf;
    float light_intensity = photodiode_current / current_at_1mW_per_cm2;
    return light_intensity; // mW/cm^2
}

static float get_light_level() {
    if (sensor_init_status != 1) {
        ESP_LOGW(TAG, "Init sensors before calling data");
        return 0x00;
    }
    return voltage_to_light_level(read_ain1());
}

sensor_data_packet_t get_sensor_data_packet(void){
    // Example packet, other values added later.
    sensor_data_packet_t ret = {
        .humidity = get_humidity(),
        .temperature = get_temperature(),
        .light = get_light_level(),
        .moisture = get_moisture(),
        .header = 0x00
    };
    
    // copy of generated packet
    return ret;
} 

