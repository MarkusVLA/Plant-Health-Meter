#pragma once

#include <stddef.h>
#include "esp_err.h"


/**
 * @brief Save Wi-Fi credentials (SSID and password) to NVS
 * 
 * @param ssid Pointer to SSID string
 * @param password Pointer to password string
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t save_wifi_credentials(const char* ssid, const char* password);

/**
 * @brief Load Wi-Fi credentials (SSID and password) from NVS
 * 
 * @param ssid Buffer to store loaded SSID
 * @param ssid_len Length of the SSID buffer
 * @param password Buffer to store loaded password
 * @param password_len Length of the password buffer
 * @return esp_err_t ESP_OK if credentials were loaded successfully, or an error code
 */
esp_err_t load_wifi_credentials(char* ssid, size_t ssid_len, char* password, size_t password_len);

/**
 * @brief Erase saved Wi-Fi credentials from NVS
 * 
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t reset_wifi_credentials(void);

/**
* @brief Save device ID string to NVS
*
* @param device_id Pointer to device ID string
* @return esp_err_t ESP_OK on success, or error code
*/
esp_err_t save_device_id(const char* device_id);

/**
* @brief Load device ID string from NVS
*
* @param device_id Buffer to store device ID
* @param id_len Length of the buffer
* @return esp_err_t ESP_OK on success, or error code
*/
esp_err_t load_device_id(char* device_id, size_t id_len);
