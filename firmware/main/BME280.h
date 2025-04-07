#pragma once
#include "esp_err.h"
#include "i2c_master.h"

// Read and check chip id
esp_err_t bme280_hand_shake(void);

// Initialize and check the bme280
esp_err_t init_bme280(void);

// Read temperature from BME280 (in °C)
esp_err_t bme280_read_temperature(float* temp);

// Read humidity from BME280 (in %RH)
esp_err_t bme280_read_humidity(float* humidity);
