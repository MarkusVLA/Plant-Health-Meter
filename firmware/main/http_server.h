#pragma once

#include "esp_http_server.h"
#include "wifi_api.h"
#include "nvs_flash.h"
#include "string.h"
#include "stdio.h"

#define JSON_PACKET_LEN 256

httpd_handle_t start_webserver();

void stop_webserver(httpd_handle_t server);
