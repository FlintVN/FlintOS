#include "hal/audio_driver.hpp"

#include <algorithm>

#include "core/logger.hpp"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace flintos {
namespace {

constexpr int I2SBclkGpio = 15;
constexpr int I2SWsGpio = 16;
constexpr int I2SDoutGpio = 7;
constexpr uint32_t SampleRate = 24000;
constexpr int GainQ15_75 = 24576; // 0.75 in Q15 format
constexpr int GainQ15_100 = 32768; // 1.0 in Q15 format

i2s_chan_handle_t audioTxChannel = nullptr;

} // namespace

extern const uint32_t bootSoundSampleRate;
extern const std::size_t bootSoundSampleCount;
extern const int16_t bootSoundPcm[];

namespace {

void writePcm(const int16_t* samples, std::size_t sampleCount) {
    if (audioTxChannel == nullptr || samples == nullptr) {
        return;
    }

    constexpr std::size_t FrameSamples = 240;
    int16_t stereo[FrameSamples * 2] = {};

    for (std::size_t offset = 0; offset < sampleCount; offset += FrameSamples) {
        const std::size_t count = std::min<std::size_t>(FrameSamples, sampleCount - offset);
        for (std::size_t index = 0; index < count; ++index) {
            const int16_t sample = static_cast<int16_t>((static_cast<int32_t>(samples[offset + index]) * GainQ15_100) >> 15);
            stereo[index * 2] = sample;
            stereo[index * 2 + 1] = sample;
        }

        size_t bytesWritten = 0;
        i2s_channel_write(audioTxChannel, stereo, count * 2 * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    }
}

void writeSilence(uint32_t durationMs) {
    if (audioTxChannel == nullptr) {
        return;
    }

    constexpr std::size_t FrameSamples = 240;
    int16_t stereo[FrameSamples * 2] = {};
    const uint32_t totalSamples = SampleRate * durationMs / 1000;
    for (uint32_t offset = 0; offset < totalSamples; offset += FrameSamples) {
        const uint32_t count = std::min<uint32_t>(FrameSamples, totalSamples - offset);
        size_t bytesWritten = 0;
        i2s_channel_write(audioTxChannel, stereo, count * 2 * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    }
}

} // namespace

bool AudioDriver::initialized_ = false;

bool AudioDriver::initialize(uint32_t sampleRate) {
    if (initialized_) {
        return audioTxChannel != nullptr;
    }
    initialized_ = true;

    i2s_chan_config_t channelConfig = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    channelConfig.auto_clear = true;
    if (i2s_new_channel(&channelConfig, &audioTxChannel, nullptr) != ESP_OK) {
        Logger::warn("I2S audio init failed");
        audioTxChannel = nullptr;
        return false;
    }

    i2s_std_config_t standardConfig = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sampleRate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = static_cast<gpio_num_t>(I2SBclkGpio),
            .ws = static_cast<gpio_num_t>(I2SWsGpio),
            .dout = static_cast<gpio_num_t>(I2SDoutGpio),
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    if (i2s_channel_init_std_mode(audioTxChannel, &standardConfig) != ESP_OK || i2s_channel_enable(audioTxChannel) != ESP_OK) {
        Logger::warn("I2S audio start failed");
        audioTxChannel = nullptr;
        return false;
    }

    Logger::info("I2S boot audio ready BCLK=15 WS=16 DOUT=7");
    return true;
}

void AudioDriver::playBootSound() {
    if (!initialize(bootSoundSampleRate)) {
        return;
    }

    writePcm(bootSoundPcm, bootSoundSampleCount);
    writeSilence(20);
}

} // namespace flintos
