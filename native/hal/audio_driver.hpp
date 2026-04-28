#pragma once

#include <cstddef>
#include <cstdint>

namespace flintos {

class AudioDriver {
public:
    bool initialize(uint32_t sampleRate = 24000);
    void playBootSound();
    bool playMusicTestTone();

private:
    static bool initialized_;
};

} // namespace flintos
