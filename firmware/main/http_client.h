// HTTP client api. TODO implement other REST methods

#pragma once


/**
 * Performs a generic HTTP PUT request to the given URL with a JSON payload.
 *
 * @param url           Full URL to send the request to
 * @param json_payload  JSON payload string
 */
void http_client_put(const char *url, const char *json_payload);
