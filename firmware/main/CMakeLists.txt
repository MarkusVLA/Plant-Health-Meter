idf_component_register(SRCS "firebase_api.c" "main.c" "wifi_api.c" "http_server.c" "http_client.c" "io_config.c" "i2c_master.c" "sensor_api.c" "packet.c"
                      INCLUDE_DIRS ""
                      REQUIRES driver esp_wifi esp_event esp_http_server
                      PRIV_REQUIRES nvs_flash esp_netif esp_timer esp_system esp_hw_support esp_http_client mbedtls)
