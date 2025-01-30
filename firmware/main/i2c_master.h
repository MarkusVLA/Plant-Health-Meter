// This file contains an API defenition for the Boch bme280 temerature/humitu/barometric sensor
// I2C master defenition
#pragma once

#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define BME_280_ADDR = 0x76 // SDO pulled low with 0 Ohm resistor on board
#define I2C_PORT 1 // Is port 1 fine?
                   //
        
// Load default i2c config on the esp32
esp_err_t begin_i2c(void);



