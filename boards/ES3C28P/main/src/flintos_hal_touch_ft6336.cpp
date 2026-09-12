
#include <algorithm>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "flint_system_api.h"
#include "flintos_default_conf.h"
#include "flintos_hal_touch_ft6336.h"

#define I2C_PORT_NUM            I2C_NUM_0

#define TOUCH_SDA_PIN           16
#define TOUCH_SCL_PIN           15
#define TOUCH_RST_PIN           18
#define TOUCH_INT_PIN           17

#define FT6336_ADDR             0x38
#define FT6336_REG_TD_STATUS    0x02

static i2c_master_dev_handle_t i2cTouchDev = NULL;

static void GPIO_Init(void) {
    gpio_config_t ioCfg = {};
    ioCfg.intr_type = GPIO_INTR_DISABLE;
    ioCfg.mode = GPIO_MODE_INPUT;
    ioCfg.pin_bit_mask = (1ULL << TOUCH_INT_PIN);
    ioCfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioCfg.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&ioCfg));

    ioCfg.intr_type = GPIO_INTR_DISABLE;
    ioCfg.mode = GPIO_MODE_OUTPUT;
    ioCfg.pin_bit_mask = (1ULL << TOUCH_RST_PIN);
    ioCfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioCfg.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&ioCfg));
}

static void Touch_Reset(void) {
    gpio_set_level((gpio_num_t)TOUCH_RST_PIN, 0);
    FlintAPI::Thread::sleep(10);
    gpio_set_level((gpio_num_t)TOUCH_RST_PIN, 1);
    FlintAPI::Thread::sleep(100);
}

static bool Touch_ReadRegs(uint8_t reg, uint8_t *buff, uint32_t length) {
    return i2c_master_transmit_receive(i2cTouchDev, &reg, 1, buff, length, pdMS_TO_TICKS(20)) == ESP_OK;
}

void FT6336::init(void) const {
    i2c_master_bus_handle_t i2cBusHandle = NULL;
    GPIO_Init();
    Touch_Reset();

    if(i2c_master_get_bus_handle(I2C_PORT_NUM, &i2cBusHandle) != ESP_OK) {
        i2c_master_bus_config_t i2cMstCfg = {};
        i2cMstCfg.clk_source = I2C_CLK_SRC_DEFAULT;
        i2cMstCfg.i2c_port = I2C_PORT_NUM;
        i2cMstCfg.sda_io_num = (gpio_num_t)TOUCH_SDA_PIN;
        i2cMstCfg.scl_io_num = (gpio_num_t)TOUCH_SCL_PIN;
        i2cMstCfg.glitch_ignore_cnt = 7;
        i2cMstCfg.flags.enable_internal_pullup = true;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMstCfg, &i2cBusHandle));
    }

    i2c_device_config_t devCfg = {};
    devCfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    devCfg.device_address = FT6336_ADDR;
    devCfg.scl_speed_hz = 400000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2cBusHandle, &devCfg, &i2cTouchDev));
}

bool FT6336::read(uint16_t *x, uint16_t *y) const {
    uint8_t data[5];

    if(!Touch_ReadRegs(FT6336_REG_TD_STATUS, data, sizeof(data))) return false;
    if((data[0] & 0x0F) == 0) return false;

    uint16_t rawX = ((data[1] & 0x0F) << 8) | data[2];
    uint16_t rawY = ((data[3] & 0x0F) << 8) | data[4];

    *x = std::min(rawX, (uint16_t)(DISPLAY_WIDTH - 1));
    *y = std::min(rawY, (uint16_t)(DISPLAY_HEIGHT - 1));

    return true;
}
