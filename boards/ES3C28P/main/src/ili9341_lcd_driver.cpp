
#include <stdatomic.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "flintos_conf.h"
#include "flintos_devices.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "flint_system_api.h"
#include "driver/spi_master.h"

#define LCD_HOST                SPI2_HOST
#define LCD_CS_PIN              10
#define LCD_MOSI_PIN            11
#define LCD_SCK_PIN             12
#define LCD_MISO_PIN            13
#define LCD_LED_PIN             45
#define LCD_DC_PIN              46
#define LCD_RST_PIN             -1
#define LCD_SPI_CLK_HZ          (60 * 1000 * 1000)

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY          5000

static esp_lcd_panel_handle_t panelHandle;

static void LED_Init(void) {
    ledc_timer_config_t ledcTimer = {};
    ledcTimer.speed_mode = LEDC_MODE;
    ledcTimer.timer_num = LEDC_TIMER;
    ledcTimer.duty_resolution = LEDC_DUTY_RES;
    ledcTimer.freq_hz = LEDC_FREQUENCY;
    ledcTimer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&ledcTimer));

    ledc_channel_config_t ledcChannel = {};
    ledcChannel.speed_mode = LEDC_MODE;
    ledcChannel.channel = LEDC_CHANNEL;
    ledcChannel.timer_sel = LEDC_TIMER;
    ledcChannel.gpio_num = LCD_LED_PIN;
    ledcChannel.duty = 0;
    ledcChannel.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&ledcChannel));
}

static void LCD_Init(void) {
    spi_bus_config_t buscfg = {};
    buscfg.sclk_io_num = LCD_SCK_PIN;
    buscfg.mosi_io_num = LCD_MOSI_PIN;
    buscfg.miso_io_num = LCD_MISO_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = DISPLAY_WIDTH * 40 * sizeof(uint16_t);
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t ioHandle = NULL;
    esp_lcd_panel_io_spi_config_t ioCfg = {};
    ioCfg.dc_gpio_num = (gpio_num_t)LCD_DC_PIN;
    ioCfg.cs_gpio_num = (gpio_num_t)LCD_CS_PIN;
    ioCfg.pclk_hz = LCD_SPI_CLK_HZ;
    ioCfg.lcd_cmd_bits = 8;
    ioCfg.lcd_param_bits = 8;
    ioCfg.spi_mode = 0;
    ioCfg.trans_queue_depth = 10;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &ioCfg, &ioHandle));

    esp_lcd_panel_dev_config_t panelCfg = {};
    panelCfg.reset_gpio_num = (gpio_num_t)LCD_RST_PIN;
    panelCfg.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
    panelCfg.bits_per_pixel = 16;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(ioHandle, &panelCfg, &panelHandle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panelHandle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panelHandle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panelHandle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panelHandle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panelHandle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panelHandle, true));
}

void FDev::Display::write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    esp_lcd_panel_draw_bitmap(panelHandle, x, y, x + w, y + h, data);
}

void FDev::Display::init(void) {
    LED_Init();
    LCD_Init();
}

void FDev::Display::brightness(uint8_t value) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value * (1 << LEDC_DUTY_RES) / 100);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
