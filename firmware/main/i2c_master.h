// This file contains an API defenition for the i2c master driver 
#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_SCL_PIN         GPIO_NUM_1
#define I2C_SDA_PIN         GPIO_NUM_0

#define I2C_SCL_FREQ        100e3 // 100kHz clock out
#define I2C_PORT_NUM        I2C_NUM_0

#define AWAIT_RESPONSE      0x01
#define IGNORE_RESPONSE     0x00
                           
// Load default i2c config on the esp32
esp_err_t begin_i2c(void);

esp_err_t i2c_read_register(uint8_t i2c_addr, uint8_t reg_addr, 
                             uint8_t *data, size_t data_size);

esp_err_t i2c_write_register(uint8_t i2c_addr, uint8_t reg_addr, 
                             uint8_t *data, size_t data_size);

esp_err_t i2c_write_byte_array(uint8_t i2c_addr, uint8_t* data, size_t data_size);
