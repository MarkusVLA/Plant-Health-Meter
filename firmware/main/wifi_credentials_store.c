#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

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
