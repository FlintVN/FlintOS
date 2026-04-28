#include "hal/audio_driver.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "core/logger.hpp"
#include "hal/display_driver.hpp"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

namespace flintos {
namespace {

constexpr int I2SBclkGpio = 15;
constexpr int I2SWsGpio = 16;
constexpr int I2SDoutGpio = 7;
constexpr uint32_t SampleRate = 24000;
constexpr float Pi = 3.14159265358979323846f;
constexpr int GainQ15_100 = 32768; // 1.0 in Q15 format

i2s_chan_handle_t audioTxChannel = nullptr;
TaskHandle_t musicPlayerTask = nullptr;
bool musicPlayerStarted = false;

} // namespace

extern const uint32_t bootSoundSampleRate;
extern const std::size_t bootSoundSampleCount;
extern const int16_t bootSoundPcm[];
extern const uint32_t musicSoundSampleRate;
extern const std::size_t musicSoundSampleCount;
extern const int16_t musicSoundPcm[];

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

void writePcm(const int16_t* samples, std::size_t sampleCount, DisplayDriver* display, const char* title) {
    if (audioTxChannel == nullptr || samples == nullptr) {
        return;
    }

    constexpr std::size_t FrameSamples = 240;
    int16_t stereo[FrameSamples * 2] = {};
    uint8_t frame = 0;

    constexpr std::size_t UiUpdateSamples = 32000;
    uint32_t levelAccumulator = 0;
    std::size_t levelSamples = 0;

    for (std::size_t offset = 0; offset < sampleCount; offset += FrameSamples) {
        if (display != nullptr && offset % UiUpdateSamples == 0) {
            const uint8_t level = levelSamples == 0 ? 0 : static_cast<uint8_t>(std::min<uint32_t>(127, levelAccumulator / levelSamples / 256));
            display->drawMusicPlayer(title, frame++, level);
            levelAccumulator = 0;
            levelSamples = 0;
        }

        const std::size_t count = std::min<std::size_t>(FrameSamples, sampleCount - offset);
        for (std::size_t index = 0; index < count; ++index) {
            const int16_t sample = static_cast<int16_t>((static_cast<int32_t>(samples[offset + index]) * GainQ15_100) >> 15);
            levelAccumulator += static_cast<uint32_t>(std::abs(static_cast<int>(sample)));
            levelSamples++;
            stereo[index * 2] = sample;
            stereo[index * 2 + 1] = sample;
        }

        size_t bytesWritten = 0;
        i2s_channel_write(audioTxChannel, stereo, count * 2 * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    }
}

void writeSilence(uint32_t durationMs);

void musicPlayerLoop(void*) {
    AudioDriver audio;
    if (!audio.initialize(musicSoundSampleRate)) {
        Logger::warn("Music player audio init failed");
        musicPlayerStarted = false;
        musicPlayerTask = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    DisplayDriver display;
    Logger::info("Music player started");
    while (true) {
        writePcm(musicSoundPcm, musicSoundSampleCount, &display, "oii.mp3");
        writeSilence(80);
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

bool AudioDriver::startMusicPlayer() {
    if (musicPlayerStarted) {
        return true;
    }

    musicPlayerStarted = true;
    const BaseType_t created = xTaskCreate(musicPlayerLoop, "music_player", 4096, nullptr, tskIDLE_PRIORITY + 1, &musicPlayerTask);
    if (created != pdPASS) {
        musicPlayerStarted = false;
        musicPlayerTask = nullptr;
        Logger::warn("Music player task start failed");
        return false;
    }
    return true;
}

} // namespace flintos
