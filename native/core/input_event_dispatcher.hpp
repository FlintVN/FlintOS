#pragma once

#include "hal/input_driver.hpp"

namespace flintos {

class InputEventDispatcher {
public:
    void initialize(InputDriver& input);
    bool isInitialized() const;
    bool dispatchPendingEvent();
    const char* lastEventType() const;

private:
    bool initialized_ = false;
    InputDriver* input_ = nullptr;
    const char* lastEventType_ = "none";
};

} // namespace flintos
