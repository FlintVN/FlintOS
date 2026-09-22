
#include "flintos.h"
#include "flintos_hal_devices.h"
#include "flintos_input_service.h"

#define TOUCH_PRESS         0
#define TOUCN_RELEASE       1

void InputSrv::mainTask(void) {
    static const uint32_t delay = 20;

    const HAL::Touch *touch = HAL::Devices::touch();
    FEvent touchEvent = {.type = 2, .data = {0}};
    bool lastStatus = false;

    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        if(touch != NULL) {
            uint16_t x, y;
            bool status = touch->read(&x, &y);
            if(status || lastStatus != status) {
                if(lastStatus != status) {
                    lastStatus = status;
                    touchEvent.data[0] = !status;
                    if(status) {
                        touchEvent.data[1] = x;
                        touchEvent.data[2] = y;
                    }
                    FlintOS::postEvent(&touchEvent);
                }
                else if(touchEvent.data[1] != x || touchEvent.data[2] != y) {
                    touchEvent.data[0] = 2;
                    touchEvent.data[1] = x;
                    touchEvent.data[2] = y;
                    FlintOS::postEvent(&touchEvent);
                }
            }
        }
        int32_t remaining = delay - (uint32_t)((uint32_t)FlintAPI::System::getTimeMillis() - tick);
        if(remaining > 0)
            FlintAPI::Thread::sleep(remaining);
    }
}
