
#include "flintos.h"
#include "flintos_hal_devices.h"
#include "flintos_input_service.h"

#define TOUCH_PRESS         0
#define TOUCN_RELEASE       1

void InputSrv::mainTask(void) {
    static const uint32_t delay = 20;

    const HAL::Touch *touch = HAL::Devices::touch();
    bool lastStatus = false;
    uint16_t lastX = 0;
    uint16_t lastY = 0;

    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        if(touch != NULL) {
            uint16_t x, y;
            bool eventFlag = false;
            bool status = touch->read(&x, &y);
            if(lastStatus != status) {
                lastStatus = status;
                eventFlag = true;
                if(status) {
                    lastX = x;
                    lastY = y;
                }
            }
            else if(status && (lastX != x || lastY != y)) {
                lastX = x;
                lastY = y;
                eventFlag = true;
            }
            if(eventFlag) {
                FEvent touchEvent = {.type = 2, .data = {!status, lastX, lastY}};
                FlintOS::postEvent(&touchEvent);
            }
        }
        int32_t remaining = delay - (uint32_t)((uint32_t)FlintAPI::System::getTimeMillis() - tick);
        if(remaining > 0)
            FlintAPI::Thread::sleep(remaining);
    }
}
