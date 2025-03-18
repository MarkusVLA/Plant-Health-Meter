idf_component_register(SRCS "main.c" "wifi_api.c" "rest_api.c" "io_config.c" "i2c_master.c"
                      INCLUDE_DIRS ""
                      REQUIRES driver esp_wifi esp_event esp_http_server
                      PRIV_REQUIRES nvs_flash esp_netif esp_timer esp_system esp_hw_support)
