idf_component_register(
    SRCS
        "firebase_api.c" "main.c" "wifi_api.c" "http_server.c" "http_client.c"
        "io_config.c" "i2c_master.c" "sensor_api.c" "packet.c" "time_sync.c"
        "ble_wifi_credentials.c" "BME280.c" "ADS1115.c" "data_store.c"
    INCLUDE_DIRS "." ""
    REQUIRES driver esp_wifi esp_event esp_http_server bt
    PRIV_REQUIRES nvs_flash esp_netif esp_timer esp_system esp_hw_support esp_http_client mbedtls bt
)
