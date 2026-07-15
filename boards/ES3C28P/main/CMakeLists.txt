include(${CMAKE_CURRENT_SOURCE_DIR}/../main/FlintOS.cmake)

set(priv_requires
    fatfs
    console
    esp_adc
    driver
    esp_driver_gpio
    esp_driver_ledc
    esp_driver_i2s
    esp_driver_i2c
    esp_timer
    nvs_flash
    esp_wifi
)

if(CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH)
    list(APPEND priv_requires wear_levelling esp_partition)
endif()

idf_component_register(
    SRCS "src/main.cpp"
    SRCS "src/esp_usb_device.cpp"
    SRCS "src/esp_board_esp32_s3.cpp"
    SRCS "src/esp_usb_cdc_debugger.cpp"
    SRCS "src/esp_system_api.cpp"
    SRCS "src/esp_wifi_driver.cpp"
    SRCS "src/ili9341_lcd_driver.cpp"
    SRCS "src/es8311_audio_driver.cpp"
    SRCS "src/fatfs_system_io_api.cpp"
    SRCS "src/freertos_psram_system_thread_api.cpp"

    SRCS ${FLINTOS_SRCS}

    INCLUDE_DIRS "inc"

    INCLUDE_DIRS ${FLINTJVM_INCS}

    PRIV_REQUIRES "${priv_requires}"
)

add_compile_definitions(ESP32_S3N16RX)
add_compile_definitions(MULTI_HEAP_FREERTOS)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-address)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-volatile)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-uninitialized)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-strict-aliasing)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-fallthrough)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-cast-function-type)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-ignored-qualifiers)
