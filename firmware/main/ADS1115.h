#pragma once

#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ADS1115_registers.h"

#ifdef __cplusplus
extern "C" {
#endif

// Conversion factors (mV per count) for different PGA settings
#define ADS1115_MV_6_144V    0.187500f  // 6.144V range (187.5μV per count)
#define ADS1115_MV_4_096V    0.125000f  // 4.096V range (125μV per count)
#define ADS1115_MV_2_048V    0.062500f  // 2.048V range (62.5μV per count)
#define ADS1115_MV_1_024V    0.031250f  // 1.024V range (31.25μV per count)
#define ADS1115_MV_0_512V    0.015625f  // 0.512V range (15.625μV per count)
#define ADS1115_MV_0_256V    0.007813f  // 0.256V range (7.813μV per count)

// Default I2C address (can be changed based on your hardware setup)
#define ADS1115_I2C_ADDR     ADS111X_I2C_ADDR_GND

/**
 * @brief Initialize the ADS1115
 * 
 * This function sets the ADS1115 to a default configuration:
 * - Single-shot mode
 * - AIN0/GND input
 * - +/-2.048V range
 * - 128 samples per second
 * - Comparator disabled
 * 
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_init(void);

/**
 * @brief Configure ADS1115 to read from a specific single-ended channel
 * 
 * @param channel Channel number (0-3)
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_channel(uint8_t channel);

/**
 * @brief Set the PGA gain
 * 
 * @param gain Gain setting (use ADS111X_CFG_PGA_* values)
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_gain(uint16_t gain);

/**
 * @brief Set the data rate
 * 
 * @param rate Data rate setting (use ADS111X_CFG_DR_* values)
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_data_rate(uint16_t rate);

/**
 * @brief Set the device mode
 * 
 * @param continuous true for continuous conversion mode, false for single-shot mode
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_mode(bool continuous);

/**
 * @brief Start a conversion (for single-shot mode)
 * 
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_start_conversion(void);

/**
 * @brief Check if conversion is ready
 * 
 * @param ready Pointer to store the ready status
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_is_conversion_ready(bool *ready);

/**
 * @brief Wait for conversion to complete
 * 
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_wait_conversion(void);

/**
 * @brief Read conversion result
 * 
 * @param result Pointer to store the raw conversion result
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_read_conversion(int16_t *result);

/**
 * @brief High-level function to read voltage from a channel
 * 
 * This function performs all necessary steps to read a voltage:
 * - Sets the channel
 * - Starts a conversion
 * - Waits for conversion to complete
 * - Reads the conversion result
 * - Converts raw value to voltage
 * 
 * @param channel Channel number (0-3)
 * @param voltage Pointer to store the voltage value in volts
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_read_voltage(uint8_t channel, float *voltage);

/**
 * @brief Simple API to read from AIN0
 * 
 * @return Voltage in volts (0.0f if error occurred)
 */
float read_ain0(void);

/**
 * @brief Simple API to read from AIN1
 * 
 * @return Voltage in volts (0.0f if error occurred)
 */
float read_ain1(void);

/**
 * @brief Simple API to read from AIN2
 * 
 * @return Voltage in volts (0.0f if error occurred)
 */
float read_ain2(void);

/**
 * @brief Simple API to read from AIN3
 * 
 * @return Voltage in volts (0.0f if error occurred)
 */
float read_ain3(void);

/**
 * @brief Dump the configuration register (useful for debugging)
 * 
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_dump_config(void);

/**
 * @brief Get the current configuration
 * 
 * @param config Pointer to store the configuration value
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_get_config(uint16_t *config);

/**
 * @brief Set configuration
 * 
 * @param config Configuration value
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_config(uint16_t config);

/**
 * @brief Configure differential input mode
 * 
 * @param positive_input Positive input channel (0-3)
 * @param negative_input Negative input channel (0-3)
 * @return ESP_OK if successful, error code otherwise
 */
esp_err_t ads1115_set_differential_mode(uint8_t positive_input, uint8_t negative_input);

#ifdef __cplusplus
}
#endif

#endif /* ADS1115_H */
