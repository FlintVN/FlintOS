#pragma once

#include <cstddef>
#include <cstdint>

namespace flintos {

class AudioDriver {
public:
    bool initialize(uint32_t sampleRate = 24000);
    void playBootSound();
    bool startMusicPlayer();

private:
    static bool initialized_;
};

} // namespace flintos
