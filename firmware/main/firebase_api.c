#include "firebase_api.h"
#include "private_config.h"
#include "http_client.h"
#include <stdio.h>
#include "time_sync.h"

void send_to_firebase(const char *json_str) {
    unsigned long ts = get_unix_timestamp();

    char url[256];
    snprintf(
        url,
        sizeof(url),
        "https://%s/sensorDataTest/%lu.json?auth=%s",
        FIREBASE_HOST,
        ts,
        FIREBASE_AUTH
    );

    http_client_put(url, json_str);
}
