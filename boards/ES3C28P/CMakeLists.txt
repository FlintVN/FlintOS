# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ES3C28P_FlintOS)

fatfs_create_spiflash_image(storage ../../files FLASH_IN_PROJECT)

add_custom_command(
    TARGET app
    POST_BUILD
    COMMAND ../Tools/esptool-win64/esptool.exe
            --chip esp32s3 merge_bin --flash_mode dio --flash_size 16MB --flash_freq 80m
            -o ../build/ES3C28P_FlintOS_Merge.bin
            0x0 bootloader/bootloader.bin
            0x8000 partition_table/partition-table.bin
            0x10000 ES3C28P_FlintOS.bin
            0x190000 storage.bin
)
