
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "flint_system_api.h"
#include "flintos_devices.h"
#include "flintos_default_conf.h"
#include "esp_codec_dev_defaults.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_vol.h"
#include "esp_check.h"

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

static esp_codec_dev_handle_t codec_handle;

static void ES8311_Init(void) {
    /* I2S initialize */
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;
    i2s_chan_config_t channelCfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT_NUM, I2S_ROLE_MASTER);
    channelCfg.dma_frame_num = AUDIO_FRAME_BUF_SIZE;
    ESP_ERROR_CHECK(i2s_new_channel(&channelCfg, &tx_handle, &rx_handle));

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
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &i2sStdCfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    /* Initialize I2C peripheral */
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_master_bus_config_t i2c_mst_cfg = {};
    i2c_mst_cfg.i2c_port = I2C_PORT_NUM;
    i2c_mst_cfg.sda_io_num = (gpio_num_t)ES8311_SDA;
    i2c_mst_cfg.scl_io_num = (gpio_num_t)ES8311_SCL;
    i2c_mst_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_cfg.glitch_ignore_cnt = 7;
    i2c_mst_cfg.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_cfg, &i2c_bus_handle));

    /* Create control interface with I2C bus handle */
    audio_codec_i2c_cfg_t i2c_cfg = {};
    i2c_cfg.port = (uint8_t)I2C_PORT_NUM;
    i2c_cfg.addr = ES8311_CODEC_DEFAULT_ADDR;
    i2c_cfg.bus_handle = i2c_bus_handle;
    const audio_codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(ctrl_if);

    /* Create data interface with I2S bus handle */
    audio_codec_i2s_cfg_t i2s_cfg = {};
    i2s_cfg.port = I2S_PORT_NUM;
    i2s_cfg.rx_handle = rx_handle;
    i2s_cfg.tx_handle = tx_handle;
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    assert(data_if);

    /* Create ES8311 interface handle */
    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    assert(gpio_if);
    es8311_codec_cfg_t es8311_cfg = {};
    es8311_cfg.ctrl_if = ctrl_if;
    es8311_cfg.gpio_if = gpio_if;
    es8311_cfg.codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH;
    es8311_cfg.master_mode = false;
    es8311_cfg.use_mclk = ES8311_MCK >= 0;
    es8311_cfg.pa_pin = ES8311_EN;
    es8311_cfg.pa_reverted = true;
    es8311_cfg.hw_gain.pa_voltage = 5.0;
    es8311_cfg.hw_gain.codec_dac_voltage = 3.3;
    const audio_codec_if_t *es8311_if = es8311_codec_new(&es8311_cfg);
    assert(es8311_if);

    /* Create the top codec handle with ES8311 interface handle and data interface */
    esp_codec_dev_cfg_t dev_cfg = {};
    dev_cfg.dev_type = ESP_CODEC_DEV_TYPE_IN_OUT;
    dev_cfg.codec_if = es8311_if;
    dev_cfg.data_if = data_if;
    codec_handle = esp_codec_dev_new(&dev_cfg);
    assert(codec_handle);

    /* Specify the sample configurations and open the device */
    esp_codec_dev_sample_info_t sample_cfg = {};
    sample_cfg.bits_per_sample = 16;
    sample_cfg.channel = 1;
    sample_cfg.channel_mask = 0;
    sample_cfg.sample_rate = AUDIO_SAMPLE_RATE;
    sample_cfg.mclk_multiple = 0;
    if(esp_codec_dev_open(codec_handle, &sample_cfg) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "Open codec device failed");
        return;
    }

    esp_codec_dev_write_reg(codec_handle, 0x34, 0x00);
}

void FDev::Audio::init(void) {
    ES8311_Init();
}

uint32_t FDev::Audio::write(uint8_t *data, uint32_t length) {
    esp_codec_dev_write(codec_handle, data, length);
    return length;
}

void FDev::Audio::setVolumn(uint8_t value) {
    esp_codec_dev_set_out_vol(codec_handle, value);
}
