// TODO implement rest of the REST methods

#include "http_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>
#include "esp_crt_bundle.h"

#define TAG "HTTP_CLIENT"

void http_client_put(const char *url, const char *json_payload) {
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_payload, strlen(json_payload));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "PUT success: %s", url);
        ESP_LOGI(TAG, "Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "PUT failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}