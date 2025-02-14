// This file contains an API defenition for the Boch bme280 temerature/humitu/barometric sensor
// and ADC
// I2C master defenition
#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define BME280_ADDR 0x76 // SDO pulled low with 0 Ohm resistor on board
#define I2C_PORT 1 // Is port 1 fine?
                   //
#define I2C_SCL_PIN GPIO_NUM_1
#define I2C_SDA_PIN GPIO_NUM_0

#define I2C_SCL_FREQ 100e3 // 100kHz clock out
#define I2C_PORT_NUM I2C_NUM_1 


                           
// Load default i2c config on the esp32
esp_err_t begin_i2c(void);

// write to device register
esp_err_t i2c_write_register(uint8_t i2c_addr, 
                             uint8_t reg_addr,
                             uint8_t *data, 
                             size_t data_size);

// Read from device register
esp_err_t i2c_read_register(uint8_t i2c_addr,
                            uint8_t reg_addr,
                            uint8_t *data,
                            size_t data_size);







