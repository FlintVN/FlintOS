#pragma once

namespace flintos {

class PowerManager {
public:
    void initialize();
    bool isInitialized() const;
    void feedWatchdog();
    const char* policy() const;

private:
    bool initialized_ = false;
};

} // namespace flintos
