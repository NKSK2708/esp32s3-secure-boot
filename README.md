# ESP32-S3 Secure Boot V2 Implementation

## Project Overview

This project demonstrates the configuration, firmware signing, and offline signature verification of Secure Boot V2 on the ESP32-S3 using ESP-IDF v5.5.2.

The objective is to establish a secure firmware boot process and provide a reusable Secure Boot Manager component that can be integrated into other ESP-IDF projects.

## Secure Boot Architecture

The intended hardware boot sequence is:

1. ESP32-S3 ROM bootloader verifies the second-stage bootloader.
2. The verified second-stage bootloader executes.
3. The second-stage bootloader verifies the application firmware.
4. The verified application starts executing.

Secure Boot V2 uses RSA-3072 digital signatures and SHA-256.

## Completed Implementation

- Set up ESP-IDF v5.5.2 for ESP32-S3.
- Created and successfully built an ESP32-S3 application.
- Generated an RSA-3072 private signing key.
- Configured Secure Boot V2 and automatic firmware signing.
- Adjusted the partition table offset to `0x10000` to accommodate the signed bootloader.
- Successfully built signed bootloader and application binaries.
- Verified both signatures using `espsecure.py`.
- Developed a reusable `secure_boot_manager` component.
- Integrated the component into the main application.
- Successfully compiled the application with the reusable component.

## Verification Results

| Firmware | Result |
|---|---|
| Second-stage bootloader | RSA signature verified successfully |
| Application firmware | RSA signature verified successfully |
| Secure Boot Manager component | Build successful |

Verification was performed offline on macOS.

## Project Structure

```text
esp32s3_secure_boot/
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
├── components/
│   └── secure_boot_manager/
│       ├── CMakeLists.txt
│       ├── secure_boot_manager.c
│       └── include/
│           └── secure_boot_manager.h
└── README.md
```

The `build/` directory, local configuration files, and private signing key are excluded from Git.

## Reusable Secure Boot Manager

The `secure_boot_manager` component provides an application-level interface for checking whether hardware Secure Boot is enabled.

It uses the ESP-IDF API `esp_secure_boot_enabled()`.

The component does not perform bootloader authentication itself. Actual Secure Boot verification is handled by the ESP32-S3 ROM bootloader and ESP-IDF second-stage bootloader.

### How to Integrate the Component into Another ESP-IDF Project

#### Step 1: Copy the Component

Copy the `secure_boot_manager` folder from this repository into the `components/` directory of your new ESP-IDF project.

Example:

```text
my_esp32_project/
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── components/
    └── secure_boot_manager/
        ├── CMakeLists.txt
        ├── secure_boot_manager.c
        └── include/
            └── secure_boot_manager.h
```

#### Step 2: Configure the Component's CMake File

Inside `components/secure_boot_manager/CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "secure_boot_manager.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES bootloader_support
)
```

`PRIV_REQUIRES bootloader_support` allows the component to use ESP-IDF's Secure Boot API internally.

#### Step 3: Connect the Component to the Main Application

Inside `main/CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES secure_boot_manager
)
```

This makes the Secure Boot Manager's public functions available to the main application.

#### Step 4: Use the Secure Boot Manager

Inside `main/main.c`:

```c
#include "esp_log.h"
#include "secure_boot_manager.h"

static const char *TAG = "SECURITY";

void app_main(void)
{
    if (secure_boot_manager_is_enabled()) {
        ESP_LOGI(TAG, "Secure Boot is ENABLED");
    } else {
        ESP_LOGW(TAG, "Secure Boot is DISABLED");
    }
}
```

The `secure_boot_manager_is_enabled()` function returns:

- `true` — Hardware Secure Boot is enabled.
- `false` — Hardware Secure Boot is disabled.

This allows applications to check Secure Boot status without directly depending on ESP-IDF's underlying Secure Boot API.

### Configuring Secure Boot V2 in a New Project

**Important:** Copying the reusable component does not automatically enable Secure Boot V2.

Each project must configure Secure Boot separately.

#### Step 1: Set the ESP32-S3 Target

```bash
idf.py set-target esp32s3
```

#### Step 2: Generate a Signing Key

For development purposes:

```bash
idf.py secure-generate-signing-key --version 2 secure_boot_signing_key.pem
```

Keep the private signing key outside version control.

For an already provisioned device, use an authorized signing key trusted by that device instead of generating an unrelated key.

#### Step 3: Configure Secure Boot

```bash
idf.py menuconfig
```

Under **Security features**:

1. Enable hardware Secure Boot in the bootloader.
2. Configure Secure Boot V2.
3. Enable signing binaries during build.
4. Specify the RSA-3072 private signing key.

Under **Partition Table**, adjust the partition table offset if required.

In this project, the offset was changed from `0x8000` to `0x10000` because the signed bootloader exceeded the original allocated space.

#### Step 4: Build the Firmware

```bash
idf.py build
```

ESP-IDF builds the second-stage bootloader, application firmware, and required components, then signs the firmware according to the configured security settings.

### Offline Signature Verification

After successfully building the project, verify the signed firmware.

Use absolute file paths because `idf.py secure-verify-signature` runs the verification tool from the build directory.

#### Verify Application Firmware

```bash
idf.py secure-verify-signature --version 2 \
  --keyfile /absolute/path/to/secure_boot_signing_key.pem \
  /absolute/path/to/project/build/application.bin
```

Replace `application.bin` with your actual application binary filename.

#### Verify Second-Stage Bootloader

```bash
idf.py secure-verify-signature --version 2 \
  --keyfile /absolute/path/to/secure_boot_signing_key.pem \
  /absolute/path/to/project/build/bootloader/bootloader.bin
```

Expected successful verification output:

```text
Signature block 0 is valid (RSA).
Signature block 0 verification successful using the supplied key (RSA).
```

This confirms that the firmware's signature is valid for the supplied signing key.

Offline verification does not confirm that Secure Boot is enabled on physical hardware.

## Current Limitations

- Hardware Secure Boot has not been enabled or validated on an ESP32-S3 device.
- eFuse provisioning and on-device boot authentication remain untested.
- The reusable component has been successfully compiled but has not undergone runtime hardware testing.
- Firmware tampering detection tests have not yet been performed.
- The private signing key is intentionally excluded from the repository.

## Next Steps

1. Test signature verification failure using modified firmware copies.
2. Create a reusable Secure Boot configuration template.
3. Improve integration documentation and automation.
4. Perform hardware validation when an ESP32-S3 development board becomes available.

## Security Note

The RSA-3072 private signing key must be kept confidential and must never be committed to source control.

For production systems, use appropriate key-management and secure-provisioning procedures.

Enabling Secure Boot through eFuse configuration can be irreversible. Hardware provisioning should be performed only after reviewing Espressif's official documentation and validating the intended configuration.