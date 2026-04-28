cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(FlintOS)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(FLINTOS_STORAGE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/build/storage-root)
set(FLINTOS_APP_TARGET app)

add_custom_target(flintos_runtime_image
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/build_runtime_image.py
    BYPRODUCTS
        ${FLINTOS_STORAGE_ROOT}/FlintApp.jar
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

fatfs_create_rawflash_image(storage ${FLINTOS_STORAGE_ROOT} FLASH_IN_PROJECT)
add_dependencies(fatfs_storage_bin flintos_runtime_image)

if(TARGET ${FLINTOS_APP_TARGET})
    add_dependencies(${FLINTOS_APP_TARGET} flintos_runtime_image)
else()
    message(WARNING "ESP-IDF app target not available; FlintOS runtime image target was not attached")
endif()
