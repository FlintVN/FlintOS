
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs_fat.h"
#include "esp_usb_device.h"
#include "esp_board.h"

static void GPIO_Init(void) {

}

static void NVS_Init(void) {
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

static void FS_Init() {
    static const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 16,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .disk_status_check_enable = false,
        .use_one_fat = false
    };
    static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount_rw_wl("", "storage", &mount_config, &s_wl_handle));
}

void Board_Init(void) {
    GPIO_Init();
    NVS_Init();
    vTaskDelay(250 / portTICK_PERIOD_MS);
    USB_DeviceInit();
    esp_tusb_init_console(TINYUSB_CDC_ACM_0);
    FS_Init();
    vTaskPrioritySet(NULL, 2);
}
