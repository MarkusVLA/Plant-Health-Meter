#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ADS1115_registers.h"

// Conversion factors (mV per int) for PGA settings
#define ADS1115_MV_6_144V    0.187500f  // 6.144V range (187.5uV per count)
#define ADS1115_MV_4_096V    0.125000f  // 4.096V range (125uV per count)
#define ADS1115_MV_2_048V    0.062500f  // 2.048V range (62.5uV per count)
#define ADS1115_MV_1_024V    0.031250f  // 1.024V range (31.25uV per count)
#define ADS1115_MV_0_512V    0.015625f  // 0.512V range (15.625uV per count)
#define ADS1115_MV_0_256V    0.007813f  // 0.256V range (7.813uV per count)

// Default I2C address when ADDR pin pulled low
#define ADS1115_I2C_ADDR     ADS111X_I2C_ADDR_GND

esp_err_t ads1115_init(void);

esp_err_t ads1115_set_channel(uint8_t channel);

esp_err_t ads1115_set_gain(uint16_t gain);

esp_err_t ads1115_set_data_rate(uint16_t rate);

esp_err_t ads1115_set_mode(bool continuous);

esp_err_t ads1115_start_conversion(void);

esp_err_t ads1115_is_conversion_ready(bool *ready);

esp_err_t ads1115_wait_conversion(void);

esp_err_t ads1115_read_conversion(int16_t *result);

esp_err_t ads1115_read_voltage(uint8_t channel, float *voltage);

float read_ain0(void);

float read_ain1(void);

float read_ain2(void);

float read_ain3(void);

esp_err_t ads1115_dump_config(void);

esp_err_t ads1115_get_config(uint16_t *config);

esp_err_t ads1115_set_config(uint16_t config);

esp_err_t ads1115_set_differential_mode(uint8_t positive_input, uint8_t negative_input);
