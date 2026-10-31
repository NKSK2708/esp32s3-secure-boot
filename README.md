# ESP32-S3 Secure Boot V2 Implementation

## Project Overview

This project demonstrates the configuration, firmware signing, and offline signature verification of Secure Boot V2 on the ESP32-S3 using ESP-IDF v5.5.2.

The objective is to establish a secure firmware boot process and develop a reusable integration component for ESP-IDF projects.

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

## Verification Results

| Firmware | Result |
|---|---|
| Second-stage bootloader | RSA signature verified successfully |
| Application firmware | RSA signature verified successfully |

Verification was performed offline on macOS.

## Current Limitations

- Hardware Secure Boot has not been enabled or validated on an ESP32-S3 device.
- eFuse provisioning and on-device boot authentication remain untested.
- The private signing key is intentionally excluded from the repository.

## Next Steps

1. Develop a reusable Secure Boot integration component.
2. Test signature verification failures using modified firmware copies.
3. Document integration and configuration instructions.
4. Perform hardware validation when an ESP32-S3 development board becomes available.

## Security Note

The RSA-3072 private signing key must be kept confidential and must never be committed to source control.