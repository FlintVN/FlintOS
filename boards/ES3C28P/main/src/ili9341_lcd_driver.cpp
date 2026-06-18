
#include <stdatomic.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "flint_system_api.h"
#include "driver/spi_master.h"
#include "ili9341_lcd_driver.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY          5000

#define SPI_MAX_TRANSFER_SZ     4096
#define SPI_QUEUE_SIZE          ((240 * 320 * 2 + (SPI_MAX_TRANSFER_SZ - 1)) / SPI_MAX_TRANSFER_SZ)

#define LCD_DC(_level)          gpio_set_level((gpio_num_t)LCD_DC_PIN, _level)

static spi_device_handle_t spiHandle = NULL;
static atomic_bool spiIsBusy = false;

static void GPIO_Init(void) {
    gpio_config_t ioCfg = {};
    ioCfg.intr_type = GPIO_INTR_DISABLE;
    ioCfg.mode = GPIO_MODE_OUTPUT;
    ioCfg.pin_bit_mask = (1ULL << LCD_DC_PIN);
    ioCfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioCfg.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&ioCfg);

    LCD_DC(1);
}

static void LED_Init(void) {
    ledc_timer_config_t ledcTimer = {};
    ledcTimer.speed_mode = LEDC_MODE;
    ledcTimer.timer_num = LEDC_TIMER;
    ledcTimer.duty_resolution = LEDC_DUTY_RES;
    ledcTimer.freq_hz = LEDC_FREQUENCY;
    ledcTimer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledcTimer);

    ledc_channel_config_t ledcChannel = {};
    ledcChannel.speed_mode = LEDC_MODE;
    ledcChannel.channel = LEDC_CHANNEL;
    ledcChannel.timer_sel = LEDC_TIMER;
    ledcChannel.intr_type = LEDC_INTR_DISABLE;
    ledcChannel.gpio_num = LCD_LED_PIN;
    ledcChannel.duty = 0;
    ledcChannel.hpoint = 0;
    ledc_channel_config(&ledcChannel);
}

static void SPI_PostCb(spi_transaction_t *t) {
    if(t->user == (void *)0xFFFFFFFF)
        spiIsBusy.store(false);
}

static void SPI_Init(void) {
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = LCD_MOSI_PIN;
    buscfg.miso_io_num = LCD_MISO_PIN;
    buscfg.sclk_io_num = LCD_SCK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = SPI_MAX_TRANSFER_SZ;
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 60000000;
    devcfg.mode = 0;
    devcfg.spics_io_num = LCD_CS_PIN;
    devcfg.queue_size = SPI_QUEUE_SIZE;
    devcfg.flags = 0;
    devcfg.post_cb = SPI_PostCb;
    spi_bus_add_device(SPI2_HOST, &devcfg, &spiHandle);
}

static void SPI_Write(uint8_t b, bool keepCs) {
    spi_transaction_t t = {};
    t.tx_data[0] = b;
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA | (keepCs ? SPI_TRANS_CS_KEEP_ACTIVE : 0);
    t.length = 8 * 1;
    spi_device_polling_transmit(spiHandle, &t);
}

static void SPI_Write(uint8_t b1, uint8_t b2) {
    spi_transaction_t t = {};
    t.tx_data[0] = b1;
    t.tx_data[1] = b2;
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8 * 2;
    spi_device_polling_transmit(spiHandle, &t);
}

static void SPI_Write(uint8_t b1, uint8_t b2, uint8_t b3) {
    spi_transaction_t t = {};
    t.tx_data[0] = b1;
    t.tx_data[1] = b2;
    t.tx_data[2] = b3;
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8 * 3;
    spi_device_polling_transmit(spiHandle, &t);
}

static void SPI_Write(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
    spi_transaction_t t = {};
    t.tx_data[0] = b1;
    t.tx_data[1] = b2;
    t.tx_data[2] = b3;
    t.tx_data[3] = b4;
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8 * 4;
    spi_device_polling_transmit(spiHandle, &t);
}

static void SPI_Write(uint8_t *data, uint32_t len, bool keepCs) {
    spi_transaction_t t = {};
    t.length = len * 8;
    t.tx_buffer = data;
    t.flags = keepCs ? SPI_TRANS_CS_KEEP_ACTIVE : 0;
    spi_device_polling_transmit(spiHandle, &t);
}

static void SPI_WriteCmd(uint8_t cmd) {
    LCD_DC(0);
    SPI_Write(cmd, false);
    LCD_DC(1);
}

static void SPI_WriteCmd(uint8_t cmd, uint8_t b) {
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(cmd, true);
    LCD_DC(1);
    SPI_Write(b, false);
    spi_device_release_bus(spiHandle);
}

static void SPI_WriteCmd(uint8_t cmd, uint8_t b1, uint8_t b2) {
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(cmd, true);
    LCD_DC(1);
    SPI_Write(b1, b2);
    spi_device_release_bus(spiHandle);
}

static void SPI_WriteCmd(uint8_t cmd, uint8_t b1, uint8_t b2, uint8_t b3) {
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(cmd, true);
    LCD_DC(1);
    SPI_Write(b1, b2, b3);
    spi_device_release_bus(spiHandle);
}

static void SPI_WriteCmd(uint8_t cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(cmd, true);
    LCD_DC(1);
    SPI_Write(b1, b2, b3, b4);
    spi_device_release_bus(spiHandle);
}

static void SPI_WriteCmd(uint8_t cmd, uint8_t *data, uint32_t length) {
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(cmd, true);
    LCD_DC(1);
    SPI_Write(data, length, false);
    spi_device_release_bus(spiHandle);
}

static void LCD_Clear(void) {
    static const uint8_t data[256] = {};

    /* Column address set */
    SPI_WriteCmd(0x2A, 0, 0, (240 >> 8), 240);

    /* Row address set */
    SPI_WriteCmd(0x2B, 0, 0, (320 >> 8), (uint8_t)320);

    /* Write to RAM */
    spi_device_acquire_bus(spiHandle, portMAX_DELAY);
    LCD_DC(0);
    SPI_Write(0x2C, true);
    LCD_DC(1);
    uint32_t n = (240 * 320 * 2) / sizeof(data);
    for(uint32_t i = 0; i < n;) {
        i++;
        SPI_Write((uint8_t *)data, sizeof(data), i < n ? true : false);
    }
    spi_device_release_bus(spiHandle);
}

bool LCD_Write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    static spi_transaction_t trans[SPI_QUEUE_SIZE] = {};
    bool expected = false;

    if(spiIsBusy.compare_exchange_strong(expected, true)) {
        /* Column address set */
        uint16_t tmp = x + w - 1;
        SPI_WriteCmd(0x2A, (x >> 8), x, (tmp >> 8), tmp);

        /* Row address set */
        tmp = y + h - 1;
        SPI_WriteCmd(0x2B, (y >> 8), y, (tmp >> 8), tmp);

        /* Write to RAM */
        uint32_t count = 0;
        uint32_t len = w * h * 2;
        spi_device_acquire_bus(spiHandle, portMAX_DELAY);
        LCD_DC(0);
        SPI_Write(0x2C, true);
        LCD_DC(1);
        while(len > 0) {
            uint32_t sz = len > SPI_MAX_TRANSFER_SZ ? SPI_MAX_TRANSFER_SZ : len;
            trans[count].length = sz * 8;
            trans[count].tx_buffer = data;
            len -= sz;
            trans[count].user = len > 0 ? 0 : (void *)0xFFFFFFFF;
            trans[count].flags = len > 0 ? SPI_TRANS_CS_KEEP_ACTIVE : 0;
            spi_device_queue_trans(spiHandle, &trans[count], portMAX_DELAY);
            count++;
            data += sz;
        }
        spi_device_release_bus(spiHandle);
        return true;
    }
    return false;
}

void LCD_Init(void) {
    GPIO_Init();
    LED_Init();
    SPI_Init();

    /* SOFTWARE RESET */
    SPI_WriteCmd(0x01);
    FlintAPI::Thread::sleep(10);
    SPI_WriteCmd(0xCF, 0x00, 0xC1, 0x30);
	SPI_WriteCmd(0xED, 0x64, 0x03, 0X12, 0X81);
	SPI_WriteCmd(0xE8, 0x85, 0x00, 0x78);
	SPI_WriteCmd(0xCB, (uint8_t []) {0x39, 0x2C, 0x00, 0x34, 0x02}, 5);
	SPI_WriteCmd(0xF7, 0x20);
	SPI_WriteCmd(0xEA, (uint8_t)0x00, 0x00);
    // Power control, VRH[5:0]
	SPI_WriteCmd(0xC0, 0x13);
    // Power control, SAP[2:0], BT[3:0]
	SPI_WriteCmd(0xC1, 0x13);
    // VCM control
	SPI_WriteCmd(0xC5, 0x22, 0x35);
    // VCM control2
	SPI_WriteCmd(0xC7, 0xBD);
	SPI_WriteCmd(0x21);
    // Memory Access Control
	SPI_WriteCmd(0x36, 0x08);
	SPI_WriteCmd(0xB6, 0x0A, 0xA2);
	SPI_WriteCmd(0x3A, 0x55);
    // Interface Control
	SPI_WriteCmd(0xF6, 0x01, 0x30);
    // VCM control
	SPI_WriteCmd(0xB1, (uint8_t)0x00, 0x1B);
    // Gamma Function Disable
	SPI_WriteCmd(0xF2, 0x00);
    // Gamma curve selected
	SPI_WriteCmd(0x26, 0x01);
    // Set Gamma
	SPI_WriteCmd(0xE0, (uint8_t []) {0x0F, 0x35, 0x31, 0x0B, 0x0E, 0x06, 0x49, 0xA7, 0x33, 0x07, 0x0F, 0x03, 0x0C, 0x0A, 0x00}, 15);
    // Set Gamma
	SPI_WriteCmd(0XE1, (uint8_t []) {0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58, 0x4D, 0x07, 0x10, 0x0C, 0x32, 0x34, 0x0F}, 15);
    /* Exit sleep */
    SPI_WriteCmd(0x11);
    /* Turn on display */
    SPI_WriteCmd(0x29);

    LCD_Clear();
}

void LCD_Brightness(uint8_t value) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value * (1 << LEDC_DUTY_RES) / 100);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
