idf_component_register(
    SRCS "secure_boot_manager.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES bootloader_support
)