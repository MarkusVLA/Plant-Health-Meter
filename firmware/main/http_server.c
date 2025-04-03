#include "http_server.h"
#include "sensor_api.h"
#include "packet.h"
#include "driver/gpio.h"
#include "io_config.h"
#include "i2c_master.h"

// HELPERS

static esp_err_t get_handler(httpd_req_t *req) {
    gpio_set_level(LED_PIN, 1);

    char response[JSON_PACKET_LEN];
    sensor_data_packet_t data_packet = get_sensor_data_packet();
    packet_to_json(response, JSON_PACKET_LEN, &data_packet);
     
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    gpio_set_level(LED_PIN, 0);
    return ESP_OK;
}

#ifndef MIN
    #define MIN(a, b) (a < b ? a: b)
#endif // MIN
       
static esp_err_t post_handler(httpd_req_t *req){
    char content[256];
    memset(content, 0x00, 256);
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);
    
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        } 
        return ESP_FAIL; 
    }

    // Print post request data
    printf("Post request data:\n%s\n", content);
    // Write post reuqest data to i2c bus
    i2c_write_byte_array(0x00, (unsigned char*) content, strlen(content));
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static httpd_uri_t uri_get = {
    .uri = "/uri",
    .method = HTTP_GET,
    .handler = get_handler, 
    .user_ctx = NULL
};

static httpd_uri_t uri_post = {
    .uri = "/uri",
    .method = HTTP_POST,
    .handler = post_handler,
    .user_ctx = NULL
};

// API

httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    return server;
}

void stop_webserver(httpd_handle_t server) {
    if (server != NULL) {
        httpd_stop(server);
    }
}
