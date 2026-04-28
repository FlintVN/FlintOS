#include "hal/audio_driver.hpp"

#include <algorithm>
#include <cmath>

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
constexpr float Pi = 3.14159265358979323846f;
constexpr int GainQ15_100 = 32768; // 1.0 in Q15 format

i2s_chan_handle_t audioTxChannel = nullptr;

} // namespace

extern const uint32_t bootSoundSampleRate;
extern const std::size_t bootSoundSampleCount;
extern const int16_t bootSoundPcm[];

namespace {

int16_t toneSample(float frequency, uint32_t index, uint32_t sampleRate, float envelope) {
    const float phase = 2.0f * Pi * frequency * static_cast<float>(index) / static_cast<float>(sampleRate);
    const float value = std::sin(phase) * envelope;
    return static_cast<int16_t>(static_cast<int32_t>(value * 32767.0f) * GainQ15_100 >> 15);
}

void writeTone(float frequency, uint32_t durationMs) {
    if (audioTxChannel == nullptr) {
        return;
    }

    constexpr std::size_t FrameSamples = 240;
    int16_t stereo[FrameSamples * 2] = {};
    const uint32_t totalSamples = SampleRate * durationMs / 1000;

    for (uint32_t offset = 0; offset < totalSamples; offset += FrameSamples) {
        const uint32_t count = std::min<uint32_t>(FrameSamples, totalSamples - offset);
        for (uint32_t index = 0; index < count; ++index) {
            const uint32_t absolute = offset + index;
            float envelope = 1.0f;
            if (absolute < SampleRate / 100) {
                envelope = static_cast<float>(absolute) / static_cast<float>(SampleRate / 100);
            } else if (totalSamples - absolute < SampleRate / 80) {
                envelope = static_cast<float>(totalSamples - absolute) / static_cast<float>(SampleRate / 80);
            }
            const int16_t sample = toneSample(frequency, absolute, SampleRate, envelope);
            stereo[index * 2] = sample;
            stereo[index * 2 + 1] = sample;
        }

        size_t bytesWritten = 0;
        i2s_channel_write(audioTxChannel, stereo, count * 2 * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    }
}

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

    writeTone(523.25f, 85);
    writeTone(659.25f, 85);
    writeTone(783.99f, 140);
    writeSilence(20);
}

void AudioDriver::playMusicTestTone() {
    if (!initialize(bootSoundSampleRate)) {
        return;
    }

    writePcm(bootSoundPcm, bootSoundSampleCount);
    writeSilence(20);
}

} // namespace flintos
