#include "core/input_event_dispatcher.hpp"

#include "core/logger.hpp"

namespace flintos {

void InputEventDispatcher::initialize(InputDriver& input) {
    input_ = &input;
    lastEventType_ = "none";
    initialized_ = true;
}

bool InputEventDispatcher::isInitialized() const {
    return initialized_;
}

bool InputEventDispatcher::dispatchPendingEvent() {
    if (!initialized_ || input_ == nullptr || !input_->hasPendingEvent()) {
        return false;
    }

    lastEventType_ = input_->readEventType();
    Logger::info("Dispatching input event to Java app event queue");
    Logger::info(lastEventType_);
    return true;
}

const char* InputEventDispatcher::lastEventType() const {
    return lastEventType_;
}

} // namespace flintos
