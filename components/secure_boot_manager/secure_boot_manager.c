#include "secure_boot_manager.h"
#include "esp_secure_boot.h"

bool secure_boot_manager_is_enabled(void)
{
    return esp_secure_boot_enabled();
}