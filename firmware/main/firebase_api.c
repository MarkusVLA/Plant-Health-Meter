#include "firebase_api.h"
#include "private_config.h"
#include "http_client.h"
#include <stdio.h>
#include "time_sync.h"

void send_to_firebase(unsigned long timestamp, const char *json_str) {
    char url[256];
    snprintf(url, sizeof(url),
             "https://%s/sensorData/%lu.json?auth=%s",
             FIREBASE_HOST, timestamp, FIREBASE_AUTH);

    http_client_put(url, json_str);
}

void send_firebase_test_payload(void) {
    unsigned long ts = get_unix_timestamp();

    // Test JSON
    const char *json = "{\"test\": \"ok\"}";

    // Build URL
    char url[256];
    snprintf(url, sizeof(url),
             "https://%s/testPayload/%lu.json?auth=%s",
             FIREBASE_HOST, ts, FIREBASE_AUTH);

    http_client_put(url, json);
}