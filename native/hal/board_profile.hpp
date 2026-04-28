#pragma once

namespace flintos {

enum class DisplayBusType {
    Auto,
    I2C,
    SPI,
    None,
};

enum class DisplayController {
    Auto,
    SSD1306,
    SH1106,
    ST7789,
    ILI9341,
    None,
};

struct DisplayProfile {
    DisplayBusType bus = DisplayBusType::Auto;
    DisplayController controller = DisplayController::Auto;
    int width = 128;
    int height = 64;
    int i2cPort = 0;
    int i2cSda = 41;
    int i2cScl = 42;
    int i2cReset = -1;
    int spiHost = 2;
    int spiMosi = -1;
    int spiMiso = -1;
    int spiSclk = -1;
    int spiCs = -1;
    int spiDc = -1;
    int spiReset = -1;
    int backlight = -1;
};

class BoardProfile {
public:
    const char* name() const;
    const char* target() const;
    bool hasPsram() const;
    unsigned flashSizeMb() const;
    DisplayProfile display() const;
};

} // namespace flintos
