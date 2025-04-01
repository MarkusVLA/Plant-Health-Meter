#pragma once

/**
 * Sends a JSON-formatted sensor payload to Firebase under the path:
 * /sensorData/<timestamp>.json
 *
 * @param timestamp    A UNIX timestamp or millisecond value for the data key
 * @param json_str     A null-terminated JSON string payload
 */
void send_to_firebase(unsigned long timestamp, const char *json_str);

void send_firebase_test_payload(void);