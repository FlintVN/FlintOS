
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
#include "wear_levelling.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

#define FLASH_PARTITION_LABEL  "storage"

#define SD_MOUNT_POINT         "/mnt/sd0"

#define SD_PIN_CLK             GPIO_NUM_38
#define SD_PIN_CMD             GPIO_NUM_40
#define SD_PIN_D0              GPIO_NUM_39
#define SD_PIN_D1              GPIO_NUM_41
#define SD_PIN_D2              GPIO_NUM_48
#define SD_PIN_D3              GPIO_NUM_47

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
    static const esp_vfs_fat_mount_config_t flash_mount_config = {
        .format_if_mount_failed = true,
        .max_files = 0,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .disk_status_check_enable = false,
        .use_one_fat = false
    };

    wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount_rw_wl("", FLASH_PARTITION_LABEL, &flash_mount_config, &s_wl_handle));
}

static void SD_Init() {
    static const esp_vfs_fat_mount_config_t sd_mount_config = {
        .format_if_mount_failed = false,
        .max_files = 0,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .disk_status_check_enable = false,
        .use_one_fat = false
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = SD_PIN_CLK;
    slot_config.cmd = SD_PIN_CMD;
    slot_config.d0 = SD_PIN_D0;
    slot_config.d1 = SD_PIN_D1;
    slot_config.d2 = SD_PIN_D2;
    slot_config.d3 = SD_PIN_D3;
    slot_config.width = 4;

    sdmmc_card_t *s_sd_card = nullptr;
    esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot_config, &sd_mount_config, &s_sd_card);
}

void Board_Init(void) {
    GPIO_Init();
    NVS_Init();
    vTaskDelay(250 / portTICK_PERIOD_MS);
    USB_DeviceInit();
    esp_tusb_init_console(TINYUSB_CDC_ACM_0);
    FS_Init();
    SD_Init();
    vTaskPrioritySet(NULL, 2);
}
