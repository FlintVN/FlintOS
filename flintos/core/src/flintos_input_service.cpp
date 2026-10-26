
#include "flintos.h"
#include "flintos_hal_devices.h"
#include "flintos_input_service.h"

#define TOUCH_PRESS         0
#define TOUCH_RELEASE       1

static void touchTask(uint32_t tick) {
    static const uint32_t delay = 20;
    static uint32_t startTime = (uint32_t)FlintAPI::System::getTimeMillis();
    static FEvent touchEvent = {.type = TOUCH_EVENT, .data = {0}};
    static bool lastStatus = false;

    const HAL::Touch *touch = HAL::Devices::touch();
    if (touch != NULL && (uint32_t)(tick - startTime) >= delay) {
        uint16_t x, y;
        bool status = touch->read(&x, &y);
        if (status || lastStatus != status) {
            if (lastStatus != status) {
                lastStatus = status;
                touchEvent.data[0] = !status;
                if (status) {
                    touchEvent.data[1] = x;
                    touchEvent.data[2] = y;
                }
                FlintOS::postEvent(&touchEvent);
            }
            else if (touchEvent.data[1] != x || touchEvent.data[2] != y) {
                touchEvent.data[0] = 2;
                touchEvent.data[1] = x;
                touchEvent.data[2] = y;
                FlintOS::postEvent(&touchEvent);
            }
        }
        startTime = tick;
    }
}

static void keyBoardTask(uint32_t tick) {
    (void)tick;

    const HAL::KeyBoard *keyBoard = HAL::Devices::keyBoard();
    if (keyBoard != NULL) {
        uint32_t keyCode;
        HAL::KeyState state = keyBoard->read(&keyCode);
        if (state != HAL::KEY_NONE) {
            FEvent keyEvent = {.type = KEY_EVENT, .data = {state, keyCode}};
            FlintOS::postEvent(&keyEvent);
        }
    }
}

void InputSrv::mainTask(void) {
    while (true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        touchTask(tick);
        keyBoardTask(tick);
        FlintAPI::Thread::sleep(2);
    }
}
