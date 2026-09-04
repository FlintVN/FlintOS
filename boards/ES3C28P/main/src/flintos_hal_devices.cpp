
#include "flintos_hal_devices.h"
#include "flintos_hal_wifi_esp.h"
#include "flintos_hal_audio_es8311.h"
#include "flintos_hal_display_ili9341.h"

using namespace HAL;

const WiFi *Devices::wifi(void) {
    static constexpr EspWiFi wifi;
    return &wifi;
}

const Audio *Devices::audio(void) {
    static constexpr ES8311 audio;
    return &audio;
}

const Display *Devices::display(void) {
    static constexpr ILI9341 display;
    return &display;
}
