#include <stdio.h>
#include "esp_log.h"
#include "secure_boot_manager.h"

static const char *TAG = "SECURE_BOOT";

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Application Started");

    if (secure_boot_manager_is_enabled()) {
        ESP_LOGI(TAG, "Secure Boot is ENABLED");
    } else {
        ESP_LOGW(TAG, "Secure Boot is DISABLED");
    }
}