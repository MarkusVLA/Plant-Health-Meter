#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "data_store.h"
#include "wifi_api.h"
#include "driver/gpio.h"

#define TAG "BLE_WIFI"

#define WIFI_SERVICE_UUID     0x00F1
#define WIFI_CHAR_UUID        0xF1F1
#define WIFI_CHAR_VAL_LEN_MAX 128

#define BLINK_GPIO 20

static uint16_t wifi_service_handle = 0;
static uint16_t wifi_char_handle = 0;
static esp_gatt_char_prop_t wifi_property = 0;

static uint8_t service_uuid_128[16] = {
    // LSB -> MSB (reverse of Bluetooth UUID)
    0xfb, 0x34, 0x9b, 0x5f,
    0x80, 0x00,
    0x00, 0x80,
    0x00, 0x10,
    0x00, 0x00,
    0xF1, 0x00, 0x00, 0x00  // <- your 0x00F1 UUID encoded here
};

static esp_attr_value_t wifi_char_val = {
    .attr_max_len = WIFI_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(uint8_t),
    .attr_value   = (uint8_t *)"\0",
};

static uint8_t service_uuid[2] = { (WIFI_SERVICE_UUID >> 8) & 0xFF, WIFI_SERVICE_UUID & 0xFF };

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid_128),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid_128),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void blink_task(void *pvParameter) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BLINK_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    while (1) {
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising data set, starting advertising");
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising failed to start");
            } else {
                ESP_LOGI(TAG, "Advertising started");
            }
            break;
        default:
            break;
    }
}

static void gatt_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            esp_ble_gap_set_device_name("PLANT_SENSOR_01");

            esp_err_t adv_ret = esp_ble_gap_config_adv_data(&adv_data);
            ESP_LOGI(TAG, "Adv data config result: %s", esp_err_to_name(adv_ret));

            esp_err_t scan_rsp_ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            ESP_LOGI(TAG, "Scan response config result: %s", esp_err_to_name(scan_rsp_ret));

            esp_gatt_srvc_id_t service_id = {
                .is_primary = true,
                .id = {
                    .inst_id = 0,
                    .uuid = {
                        .len = ESP_UUID_LEN_16,
                        .uuid = { .uuid16 = WIFI_SERVICE_UUID },
                    },
                },
            };
            esp_ble_gatts_create_service(gatts_if, &service_id, 4);
            break;

        case ESP_GATTS_CREATE_EVT:
            wifi_service_handle = param->create.service_handle;
            ESP_LOGI(TAG, "Service created: handle %d", wifi_service_handle);

            esp_bt_uuid_t char_uuid = {
                .len = ESP_UUID_LEN_16,
                .uuid = { .uuid16 = WIFI_CHAR_UUID },
            };
            wifi_property = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
            esp_ble_gatts_add_char(wifi_service_handle, &char_uuid,
                                   ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ,
                                   wifi_property, &wifi_char_val, NULL);
            break;

        case ESP_GATTS_ADD_CHAR_EVT:
            wifi_char_handle = param->add_char.attr_handle;
            ESP_LOGI(TAG, "Characteristic added: handle %d", wifi_char_handle);
            esp_ble_gatts_start_service(wifi_service_handle);
            break;

            case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep) {
                char received[param->write.len + 1];
                memcpy(received, param->write.value, param->write.len);
                received[param->write.len] = '\0';
        
                ESP_LOGI(TAG, "Received credentials: %s", received);
        
                // Split by first ':'
                char *id_token = strtok(received, ":");
                char *ssid_token = strtok(NULL, ":");
                char *password_token = strtok(NULL, "");
        
                if (id_token && ssid_token && password_token) {
                    const char *device_id = id_token;
                    const char *ssid = ssid_token;
                    const char *password = password_token;
                
                    ESP_LOGI(TAG, "Parsed Device ID: %s", device_id);
                    ESP_LOGI(TAG, "Parsed SSID: %s", ssid);
                    ESP_LOGI(TAG, "Parsed Password: %s", password);
                
                    esp_err_t save_ret = save_wifi_credentials(ssid, password);
                    esp_err_t save_id_ret = save_device_id(device_id);
                
                    if (save_ret == ESP_OK && save_id_ret == ESP_OK) {
                        ESP_LOGI(TAG, "Credentials and Device ID saved to NVS successfully");
                
                        ESP_LOGI(TAG, "Restarting device to attempt Wi-Fi connection...");
                        esp_restart();
                    } else {
                        ESP_LOGE(TAG, "Failed to save:");
                        if (save_ret != ESP_OK)
                            ESP_LOGE(TAG, " - Wi-Fi credentials: %s", esp_err_to_name(save_ret));
                        if (save_id_ret != ESP_OK)
                            ESP_LOGE(TAG, " - Device ID: %s", esp_err_to_name(save_id_ret));
                    }
                } else {
                    ESP_LOGW(TAG, "Invalid format. Expected ID:SSID:PASSWORD");
                }
            }
            break;
        
        default:
            break;
    }
}

void ble_wifi_start_server(void) {
    ESP_LOGI(TAG, "BLE server starting...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatt_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));

    xTaskCreate(blink_task, "blink_task", 1024, NULL, 5, NULL);

    ESP_LOGI(TAG, "Waiting for BLE credentials (SSID:PASSWORD)...");
}
