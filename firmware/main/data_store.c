#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>
#include <ctype.h>

#define TAG "wifi_store"
#define NVS_NAMESPACE "wifi_creds"

esp_err_t save_wifi_credentials(const char* ssid, const char* password) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    nvs_set_str(handle, "ssid", ssid);
    nvs_set_str(handle, "password", password);
    err = nvs_commit(handle);
    nvs_close(handle);

    return err;
}

esp_err_t load_wifi_credentials(char* ssid, size_t ssid_len, char* password, size_t password_len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    err = nvs_get_str(handle, "ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_str(handle, "password", password, &password_len);
    nvs_close(handle);
    return err;
}

esp_err_t reset_wifi_credentials() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        nvs_erase_all(handle);
        nvs_commit(handle);
        nvs_close(handle);
    }
    return err;
}

esp_err_t save_device_id(const char* device_id) {
    // Validate device ID: only allow [a-zA-Z0-9_]
    for (size_t i = 0; i < strlen(device_id); i++) {
        char c = device_id[i];
        if (!(isalnum((unsigned char)c) || c == '_')) {
            ESP_LOGW(TAG, "Invalid character '%c' in device_id. Only alphanumeric and underscores are allowed.", c);
            return ESP_ERR_INVALID_ARG;
        }
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_str(handle, "device_id", device_id);
    if (err == ESP_OK) err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}

esp_err_t load_device_id(char* device_id, size_t id_len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    err = nvs_get_str(handle, "device_id", device_id, &id_len);
    nvs_close(handle);
    return err;
}
