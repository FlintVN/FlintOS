#include "hal/board_profile.hpp"

namespace flintos {

const char* BoardProfile::name() const {
    return "ESP32-S3";
}

const char* BoardProfile::target() const {
    return "esp32s3";
}

bool BoardProfile::hasPsram() const {
    return true;
}

unsigned BoardProfile::flashSizeMb() const {
    return 8;
}

} // namespace flintos
