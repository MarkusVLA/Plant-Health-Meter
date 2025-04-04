#include "firebase_api.h"
#include "private_config.h"
#include "http_client.h"
#include "data_store.h"
#include <stdio.h>
#include "time_sync.h"
#include <string.h>

void send_to_firebase(const char *json_str) {
    unsigned long ts = get_unix_timestamp();

    char device_id[32] = "0"; // default if not set
    esp_err_t id_ret = load_device_id(device_id, sizeof(device_id));
    if (id_ret != ESP_OK) {
        strcpy(device_id, "0");
    }

    char url[256];
    snprintf(
        url,
        sizeof(url),
        "https://%s/sensorDataTest/%s/%lu.json?auth=%s",
        FIREBASE_HOST,
        device_id,
        ts,
        FIREBASE_AUTH
    );

    http_client_put(url, json_str);
}
