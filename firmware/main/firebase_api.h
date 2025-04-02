#pragma once

/**
 * Sends a JSON-formatted sensor payload to Firebase under the path:
 * /sensorData/<timestamp>.json
 *
 * @param json_str     A null-terminated JSON string payload
 */
void send_to_firebase(const char *json_str);