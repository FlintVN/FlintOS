#pragma once

namespace flintos {

class InputDriver {
public:
    void initialize();
    bool isInitialized() const;
    bool hasPendingEvent() const;
    const char* readEventType() const;

private:
    bool initialized_ = false;
};

} // namespace flintos
