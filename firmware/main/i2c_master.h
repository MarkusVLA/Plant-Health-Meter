// This file contains an API defenition for the i2c master driver 
#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_SCL_PIN         GPIO_NUM_22
#define I2C_SDA_PIN         GPIO_NUM_21

#define I2C_SCL_FREQ        100e3 // 100kHz clock out
#define I2C_PORT_NUM        I2C_NUM_1 

#define AWAIT_RESPONSE      0x01
#define IGNORE_RESPONSE     0x00
                           
// Load default i2c config on the esp32
esp_err_t begin_i2c(void);

// Test function to verify that i2c is working
esp_err_t test_i2c();

// Write string to i2c bus for testing purpose.
esp_err_t i2c_write_byte_array(uint8_t* data, size_t data_size);

// Read and check chip id
esp_err_t bme280_hand_shake(void);

// Initialize and check the bme280
esp_err_t init_bme280(void);

