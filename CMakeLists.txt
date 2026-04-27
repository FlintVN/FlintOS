cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(FlintOS)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(FLINTOS_STORAGE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/build/storage-root)
set(FLINTOS_STORAGE_IMAGE ${CMAKE_BINARY_DIR}/storage.bin)
set(FLINTOS_APP_TARGET app)

add_custom_target(flintos_runtime_image
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/build_runtime_image.py
    BYPRODUCTS
        ${FLINTOS_STORAGE_ROOT}/FlintApp.jar
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(TARGET ${FLINTOS_APP_TARGET})
    add_dependencies(${FLINTOS_APP_TARGET} flintos_runtime_image)

    add_custom_command(
        TARGET ${FLINTOS_APP_TARGET}
        POST_BUILD
        COMMAND ${Python3_EXECUTABLE} $ENV{IDF_PATH}/components/fatfs/wl_fatfsgen.py
                --output_file ${FLINTOS_STORAGE_IMAGE}
                --partition_size 0x160000
                --sector_size 512
                --long_name_support
                ${FLINTOS_STORAGE_ROOT}
    )
else()
    message(WARNING "ESP-IDF app target not available; FlintOS storage image post-build step was not attached")
endif()
