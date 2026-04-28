#pragma once

#include <cstddef>
#include <cstdint>

#include "hal/board_profile.hpp"

namespace flintos {

class DisplayDriver {
public:
    void initialize();
    void clear();
    void drawText(const char* text, int x, int y);
    void drawMusicPlayer(const char* title, uint8_t frame, uint8_t level = 0);
    void playBootAnimation();

private:
    static constexpr int MaxWidth = 128;
    static constexpr int MaxHeight = 64;
    static constexpr std::size_t FramebufferSize = MaxWidth * MaxHeight / 8;

    enum class Backend {
        Unknown,
        Log,
        SSD1306I2C,
    };

    bool probeI2CAddress(uint8_t address) const;
    bool initializeSSD1306(uint8_t address);
    bool sendCommand(uint8_t command) const;
    bool sendData(const uint8_t* data, std::size_t length) const;
    void flush();
    void drawChar(char value, int x, int y);
    bool pixel(int x, int y) const;
    void setPixel(int x, int y, bool enabled);

    static bool initialized_;
    static Backend backend_;
    static DisplayProfile profile_;
    static uint8_t i2cAddress_;
    static uint8_t framebuffer_[FramebufferSize];
};

} // namespace flintos
