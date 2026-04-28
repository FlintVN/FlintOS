#include "hal/display_driver.hpp"

#include <algorithm>
#include <cstring>

#include "core/logger.hpp"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace flintos {
namespace {

constexpr uint32_t I2CClockHz = 400000;
constexpr int ProbeTimeoutMs = 50;
constexpr int TransferTimeoutMs = 100;
constexpr int FontWidth = 5;
constexpr int FontHeight = 7;
constexpr int FontAdvance = 6;

struct I2CPinPair {
    int sda;
    int scl;
    const char* label;
};

constexpr I2CPinPair CommonI2CPins[] = {
    {41, 42, "SDA=41 SCL=42"},
    {8, 9, "SDA=8 SCL=9"},
    {21, 22, "SDA=21 SCL=22"},
    {17, 18, "SDA=17 SCL=18"},
    {3, 4, "SDA=3 SCL=4"},
    {5, 6, "SDA=5 SCL=6"},
};

i2c_master_bus_handle_t displayBus = nullptr;
i2c_master_dev_handle_t displayDevice = nullptr;

const uint8_t* glyphFor(char value) {
    static constexpr uint8_t space[] = {0x00, 0x00, 0x00, 0x00, 0x00};
    static constexpr uint8_t unknown[] = {0x7f, 0x41, 0x5d, 0x41, 0x7f};
    static constexpr uint8_t digits[][FontWidth] = {
        {0x3e, 0x51, 0x49, 0x45, 0x3e},
        {0x00, 0x42, 0x7f, 0x40, 0x00},
        {0x42, 0x61, 0x51, 0x49, 0x46},
        {0x21, 0x41, 0x45, 0x4b, 0x31},
        {0x18, 0x14, 0x12, 0x7f, 0x10},
        {0x27, 0x45, 0x45, 0x45, 0x39},
        {0x3c, 0x4a, 0x49, 0x49, 0x30},
        {0x01, 0x71, 0x09, 0x05, 0x03},
        {0x36, 0x49, 0x49, 0x49, 0x36},
        {0x06, 0x49, 0x49, 0x29, 0x1e},
    };
    static constexpr uint8_t uppercase[][FontWidth] = {
        {0x7e, 0x11, 0x11, 0x11, 0x7e},
        {0x7f, 0x49, 0x49, 0x49, 0x36},
        {0x3e, 0x41, 0x41, 0x41, 0x22},
        {0x7f, 0x41, 0x41, 0x22, 0x1c},
        {0x7f, 0x49, 0x49, 0x49, 0x41},
        {0x7f, 0x09, 0x09, 0x09, 0x01},
        {0x3e, 0x41, 0x49, 0x49, 0x7a},
        {0x7f, 0x08, 0x08, 0x08, 0x7f},
        {0x00, 0x41, 0x7f, 0x41, 0x00},
        {0x20, 0x40, 0x41, 0x3f, 0x01},
        {0x7f, 0x08, 0x14, 0x22, 0x41},
        {0x7f, 0x40, 0x40, 0x40, 0x40},
        {0x7f, 0x02, 0x0c, 0x02, 0x7f},
        {0x7f, 0x04, 0x08, 0x10, 0x7f},
        {0x3e, 0x41, 0x41, 0x41, 0x3e},
        {0x7f, 0x09, 0x09, 0x09, 0x06},
        {0x3e, 0x41, 0x51, 0x21, 0x5e},
        {0x7f, 0x09, 0x19, 0x29, 0x46},
        {0x46, 0x49, 0x49, 0x49, 0x31},
        {0x01, 0x01, 0x7f, 0x01, 0x01},
        {0x3f, 0x40, 0x40, 0x40, 0x3f},
        {0x1f, 0x20, 0x40, 0x20, 0x1f},
        {0x3f, 0x40, 0x38, 0x40, 0x3f},
        {0x63, 0x14, 0x08, 0x14, 0x63},
        {0x07, 0x08, 0x70, 0x08, 0x07},
        {0x61, 0x51, 0x49, 0x45, 0x43},
    };

    if (value == ' ') {
        return space;
    }
    if (value >= '0' && value <= '9') {
        return digits[value - '0'];
    }
    if (value >= 'a' && value <= 'z') {
        value = static_cast<char>(value - 'a' + 'A');
    }
    if (value >= 'A' && value <= 'Z') {
        return uppercase[value - 'A'];
    }
    return unknown;
}

void releaseI2CDevice() {
    if (displayDevice != nullptr) {
        i2c_master_bus_rm_device(displayDevice);
        displayDevice = nullptr;
    }
}

void releaseI2CBus() {
    releaseI2CDevice();
    if (displayBus != nullptr) {
        i2c_del_master_bus(displayBus);
        displayBus = nullptr;
    }
}

} // namespace

bool DisplayDriver::initialized_ = false;
DisplayDriver::Backend DisplayDriver::backend_ = DisplayDriver::Backend::Unknown;
DisplayProfile DisplayDriver::profile_ = {};
uint8_t DisplayDriver::i2cAddress_ = 0;
uint8_t DisplayDriver::framebuffer_[DisplayDriver::FramebufferSize] = {};

void DisplayDriver::initialize() {
    if (initialized_) {
        return;
    }

    profile_ = BoardProfile().display();
    backend_ = Backend::Log;

    if (profile_.bus == DisplayBusType::None || profile_.controller == DisplayController::None) {
        Logger::warn("Display fallback log backend");
        initialized_ = true;
        return;
    }

    for (const I2CPinPair& pins : CommonI2CPins) {
        profile_.i2cSda = pins.sda;
        profile_.i2cScl = pins.scl;

        i2c_master_bus_config_t busConfig = {};
        busConfig.i2c_port = static_cast<i2c_port_num_t>(profile_.i2cPort);
        busConfig.sda_io_num = static_cast<gpio_num_t>(profile_.i2cSda);
        busConfig.scl_io_num = static_cast<gpio_num_t>(profile_.i2cScl);
        busConfig.clk_source = I2C_CLK_SRC_DEFAULT;
        busConfig.glitch_ignore_cnt = 7;
        busConfig.flags.enable_internal_pullup = true;

        if (i2c_new_master_bus(&busConfig, &displayBus) != ESP_OK) {
            releaseI2CBus();
            continue;
        }

        if (probeI2CAddress(0x3c) && initializeSSD1306(0x3c)) {
            Logger::info(pins.label);
            initialized_ = true;
            return;
        }
        if (probeI2CAddress(0x3d) && initializeSSD1306(0x3d)) {
            Logger::info(pins.label);
            initialized_ = true;
            return;
        }

        releaseI2CBus();
    }

    Logger::warn("Display fallback log backend");
    initialized_ = true;
}

void DisplayDriver::clear() {
    initialize();
    std::fill(framebuffer_, framebuffer_ + FramebufferSize, 0);
    if (backend_ == Backend::SSD1306I2C) {
        flush();
        return;
    }
    Logger::info("Display clear requested");
}

void DisplayDriver::drawText(const char* text, int x, int y) {
    initialize();
    if (text == nullptr) {
        Logger::warn("Display drawText ignored null text");
        return;
    }

    if (backend_ != Backend::SSD1306I2C) {
        Logger::info(text);
        return;
    }

    int cursorX = x;
    int cursorY = y;
    for (const char* current = text; *current != '\0'; ++current) {
        if (*current == '\n') {
            cursorX = x;
            cursorY += FontHeight + 1;
            continue;
        }
        drawChar(*current, cursorX, cursorY);
        cursorX += FontAdvance;
        if (cursorX + FontWidth > profile_.width) {
            cursorX = x;
            cursorY += FontHeight + 1;
        }
        if (cursorY >= profile_.height) {
            break;
        }
    }
    flush();
}

void DisplayDriver::drawMusicPlayer(const char* title, uint8_t frame, uint8_t level) {
    initialize();
    const char* trackTitle = title == nullptr ? "unknown" : title;

    if (backend_ != Backend::SSD1306I2C) {
        Logger::info("Music Player");
        Logger::info(trackTitle);
        return;
    }

    auto drawTextBuffered = [this](const char* text, int x, int y) {
        int cursorX = x;
        int cursorY = y;
        for (const char* current = text; current != nullptr && *current != '\0'; ++current) {
            drawChar(*current, cursorX, cursorY);
            cursorX += FontAdvance;
            if (cursorX + FontWidth > profile_.width) {
                break;
            }
        }
    };

    std::fill(framebuffer_, framebuffer_ + FramebufferSize, 0);

    for (int x = 0; x < profile_.width; ++x) {
        setPixel(x, 10, true);
        setPixel(x, 54, true);
    }

    drawTextBuffered("Flint Music", 28, 1);
    drawTextBuffered(trackTitle, 8, 17);
    drawTextBuffered("PLAYING", 42, 43);

    constexpr int IconX = 56;
    constexpr int IconY = 28;
    for (int y = 0; y < 13; ++y) {
        for (int x = 0; x <= y / 2; ++x) {
            setPixel(IconX + x, IconY + y, true);
        }
    }

    const int bars[] = {3, 6, 10, 6, 3};
    const int phase = frame % 5;
    const int boost = 1 + level / 32;
    for (int index = 0; index < 5; ++index) {
        const int height = std::min(13, bars[(index + phase) % 5] + boost);
        const int x = 18 + index * 5;
        for (int y = 0; y < height; ++y) {
            setPixel(x, 39 - y, true);
            setPixel(x + 1, 39 - y, true);
        }
    }
    for (int index = 0; index < 5; ++index) {
        const int height = std::min(13, bars[(index + 5 - phase) % 5] + boost);
        const int x = 86 + index * 5;
        for (int y = 0; y < height; ++y) {
            setPixel(x, 39 - y, true);
            setPixel(x + 1, 39 - y, true);
        }
    }

    const int progress = static_cast<int>((frame % 32) * 112 / 31);
    for (int x = 8; x < 120; ++x) {
        setPixel(x, 58, true);
    }
    for (int x = 8; x < 8 + progress; ++x) {
        setPixel(x, 57, true);
        setPixel(x, 59, true);
    }

    flush();
}

void DisplayDriver::playBootAnimation() {
    initialize();
    if (backend_ != Backend::SSD1306I2C) {
        Logger::info("FlintOS");
        return;
    }

    constexpr const char* title = "FlintOS";
    constexpr int textWidth = 7 * FontAdvance - 1;
    constexpr int centerY = 24;
    constexpr int delayMs = 35;

    for (int x = -textWidth; x <= (profile_.width - textWidth) / 2; x += 4) {
        std::fill(framebuffer_, framebuffer_ + FramebufferSize, 0);
        drawText(title, x, centerY);
        vTaskDelay(pdMS_TO_TICKS(delayMs));
    }

    for (int dots = 0; dots < 4; ++dots) {
        std::fill(framebuffer_, framebuffer_ + FramebufferSize, 0);
        drawText(title, (profile_.width - textWidth) / 2, centerY);
        for (int index = 0; index < dots; ++index) {
            drawChar('.', (profile_.width + textWidth) / 2 + 4 + index * 6, centerY);
        }
        flush();
        vTaskDelay(pdMS_TO_TICKS(140));
    }
}

bool DisplayDriver::probeI2CAddress(uint8_t address) const {
    if (displayBus == nullptr) {
        return false;
    }
    return i2c_master_probe(displayBus, address, ProbeTimeoutMs) == ESP_OK;
}

bool DisplayDriver::initializeSSD1306(uint8_t address) {
    releaseI2CDevice();

    i2c_device_config_t deviceConfig = {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = address;
    deviceConfig.scl_speed_hz = I2CClockHz;
    if (i2c_master_bus_add_device(displayBus, &deviceConfig, &displayDevice) != ESP_OK) {
        return false;
    }

    i2cAddress_ = address;
    backend_ = Backend::SSD1306I2C;

    const uint8_t commands[] = {
        0xae, 0x20, 0x00, 0xb0, 0xc8, 0x00, 0x10, 0x40,
        0x81, 0x7f, 0xa1, 0xa6, 0xa8, static_cast<uint8_t>(profile_.height - 1),
        0xa4, 0xd3, 0x00, 0xd5, 0x80, 0xd9, 0xf1, 0xda,
        static_cast<uint8_t>(profile_.height == 64 ? 0x12 : 0x02),
        0xdb, 0x40, 0x8d, 0x14, 0xaf,
    };

    for (uint8_t command : commands) {
        if (!sendCommand(command)) {
            backend_ = Backend::Log;
            releaseI2CDevice();
            return false;
        }
    }

    std::fill(framebuffer_, framebuffer_ + FramebufferSize, 0);
    flush();
    Logger::info(i2cAddress_ == 0x3c ? "Display SSD1306 I2C 0x3C" : "Display SSD1306 I2C 0x3D");
    return true;
}

bool DisplayDriver::sendCommand(uint8_t command) const {
    if (displayDevice == nullptr) {
        return false;
    }
    const uint8_t packet[] = {0x00, command};
    return i2c_master_transmit(displayDevice, packet, sizeof(packet), TransferTimeoutMs) == ESP_OK;
}

bool DisplayDriver::sendData(const uint8_t* data, std::size_t length) const {
    if (displayDevice == nullptr || data == nullptr) {
        return false;
    }

    uint8_t packet[17] = {0x40};
    std::size_t offset = 0;
    while (offset < length) {
        const std::size_t chunk = std::min<std::size_t>(sizeof(packet) - 1, length - offset);
        std::memcpy(&packet[1], data + offset, chunk);
        if (i2c_master_transmit(displayDevice, packet, chunk + 1, TransferTimeoutMs) != ESP_OK) {
            return false;
        }
        offset += chunk;
    }
    return true;
}

void DisplayDriver::flush() {
    if (backend_ != Backend::SSD1306I2C) {
        return;
    }

    const int pages = profile_.height / 8;
    for (int page = 0; page < pages; ++page) {
        sendCommand(static_cast<uint8_t>(0xb0 + page));
        sendCommand(0x00);
        sendCommand(0x10);
        sendData(&framebuffer_[page * profile_.width], profile_.width);
    }
}

void DisplayDriver::drawChar(char value, int x, int y) {
    const uint8_t* glyph = glyphFor(value);
    for (int column = 0; column < FontWidth; ++column) {
        for (int row = 0; row < FontHeight; ++row) {
            setPixel(x + column, y + row, (glyph[column] & (1 << row)) != 0);
        }
    }
}

bool DisplayDriver::pixel(int x, int y) const {
    if (x < 0 || y < 0 || x >= profile_.width || y >= profile_.height) {
        return false;
    }
    const std::size_t index = static_cast<std::size_t>(x + (y / 8) * profile_.width);
    return (framebuffer_[index] & (1 << (y % 8))) != 0;
}

void DisplayDriver::setPixel(int x, int y, bool enabled) {
    if (x < 0 || y < 0 || x >= profile_.width || y >= profile_.height) {
        return;
    }
    const std::size_t index = static_cast<std::size_t>(x + (y / 8) * profile_.width);
    const uint8_t mask = static_cast<uint8_t>(1 << (y % 8));
    if (enabled) {
        framebuffer_[index] |= mask;
    } else {
        framebuffer_[index] &= static_cast<uint8_t>(~mask);
    }
}

} // namespace flintos
