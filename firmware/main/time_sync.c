#include <time.h>
#include <esp_log.h>
#include <esp_sntp.h>

#define TAG "TIME_SYNC"

void sync_time(void) {
    ESP_LOGI(TAG, "Initializing SNTP");

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    ESP_LOGI(TAG, "Waiting for system time to be set...");
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for NTP sync... (%d/%d)", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    time(&now);
    localtime_r(&now, &timeinfo);

    if (retry < retry_count) {
        ESP_LOGI(TAG, "Time synced: %s", asctime(&timeinfo));
    } else {
        ESP_LOGW(TAG, "Time sync failed.");
    }
}

unsigned long get_unix_timestamp(void) {
    time_t now;
    time(&now);
    return (unsigned long)now;
}