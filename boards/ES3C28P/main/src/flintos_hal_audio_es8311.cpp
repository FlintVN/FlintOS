
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "flint_system_api.h"
#include "flintos_default_conf.h"
#include "esp_codec_dev_defaults.h"
#include "flintos_hal_audio_es8311.h"
#include "esp_check.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_vol.h"

#define I2S_PORT_NUM            I2S_NUM_0
#define I2C_PORT_NUM            I2C_NUM_0

#define ES8311_MCK              4
#define ES8311_SCK              5
#define ES8311_SD_OUT           6
#define ES8311_SD_IN            8
#define ES8311_WS               7

#define ES8311_EN               1
#define ES8311_SCL              15
#define ES8311_SDA              16

static const char *TAG = "ES8311_INIT";
static esp_codec_dev_handle_t codecHandle;

void ES8311::init(void) const {
    /* I2S initialize */
    i2s_chan_handle_t txHandle;
    i2s_chan_handle_t rxHandle;
    i2s_chan_config_t channelCfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT_NUM, I2S_ROLE_MASTER);
    channelCfg.dma_frame_num = AUDIO_FRAME_BUF_SIZE;
    ESP_ERROR_CHECK(i2s_new_channel(&channelCfg, &txHandle, &rxHandle));

    i2s_std_config_t i2sStdCfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(AUDIO_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = (gpio_num_t)ES8311_MCK,
            .bclk = (gpio_num_t)ES8311_SCK,
            .ws   = (gpio_num_t)ES8311_WS,
            .dout = (gpio_num_t)ES8311_SD_IN,
            .din  = (gpio_num_t)ES8311_SD_OUT,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(txHandle, &i2sStdCfg));
    ESP_ERROR_CHECK(i2s_channel_enable(txHandle));

    /* Initialize I2C peripheral */
    i2c_master_bus_handle_t i2cBusHandle = NULL;
    i2c_master_bus_config_t i2cMstCfg = {};
    i2cMstCfg.i2c_port = I2C_PORT_NUM;
    i2cMstCfg.sda_io_num = (gpio_num_t)ES8311_SDA;
    i2cMstCfg.scl_io_num = (gpio_num_t)ES8311_SCL;
    i2cMstCfg.clk_source = I2C_CLK_SRC_DEFAULT;
    i2cMstCfg.glitch_ignore_cnt = 7;
    i2cMstCfg.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cMstCfg, &i2cBusHandle));

    /* Create control interface with I2C bus handle */
    audio_codec_i2c_cfg_t i2cCfg = {};
    i2cCfg.port = (uint8_t)I2C_PORT_NUM;
    i2cCfg.addr = ES8311_CODEC_DEFAULT_ADDR;
    i2cCfg.bus_handle = i2cBusHandle;
    const audio_codec_ctrl_if_t *ctrlIf = audio_codec_new_i2c_ctrl(&i2cCfg);
    assert(ctrlIf);

    /* Create data interface with I2S bus handle */
    audio_codec_i2s_cfg_t i2sCfg = {};
    i2sCfg.port = I2S_PORT_NUM;
    i2sCfg.rx_handle = rxHandle;
    i2sCfg.tx_handle = txHandle;
    const audio_codec_data_if_t *dataIf = audio_codec_new_i2s_data(&i2sCfg);
    assert(dataIf);

    /* Create ES8311 interface handle */
    const audio_codec_gpio_if_t *gpioIf = audio_codec_new_gpio();
    assert(gpioIf);
    es8311_codec_cfg_t es8311Cfg = {};
    es8311Cfg.ctrl_if = ctrlIf;
    es8311Cfg.gpio_if = gpioIf;
    es8311Cfg.codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH;
    es8311Cfg.master_mode = false;
    es8311Cfg.use_mclk = ES8311_MCK >= 0;
    es8311Cfg.pa_pin = ES8311_EN;
    es8311Cfg.pa_reverted = true;
    es8311Cfg.hw_gain.pa_voltage = 5.0;
    es8311Cfg.hw_gain.codec_dac_voltage = 3.3;
    const audio_codec_if_t *es8311If = es8311_codec_new(&es8311Cfg);
    assert(es8311If);

    /* Create the top codec handle with ES8311 interface handle and data interface */
    esp_codec_dev_cfg_t devCfg = {};
    devCfg.dev_type = ESP_CODEC_DEV_TYPE_IN_OUT;
    devCfg.codec_if = es8311If;
    devCfg.data_if = dataIf;
    codecHandle = esp_codec_dev_new(&devCfg);
    assert(codecHandle);

    /* Specify the sample configurations and open the device */
    esp_codec_dev_sample_info_t sampleCfg = {};
    sampleCfg.bits_per_sample = 16;
    sampleCfg.channel = 1;
    sampleCfg.channel_mask = 0;
    sampleCfg.sample_rate = AUDIO_SAMPLE_RATE;
    sampleCfg.mclk_multiple = 0;
    if(esp_codec_dev_open(codecHandle, &sampleCfg) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "Open codec device failed");
        return;
    }

    esp_codec_dev_write_reg(codecHandle, 0x34, 0x00);
}

uint32_t ES8311::write(uint8_t *data, uint32_t length) const {
    esp_codec_dev_write(codecHandle, data, length);
    return length;
}

void ES8311::setVolumn(uint8_t value) const {
    esp_codec_dev_set_out_vol(codecHandle, value);
}
