
#include <stddef.h>
#include <string.h>
#include <algorithm>
#include <stdatomic.h>
#include "flintos.h"
#include "flintos_logo.h"
#include "flint_system_api.h"
#include "flint_file_reader.h"
#include "flintos_hal_devices.h"
#include "flintos_default_conf.h"
#include "flintos_display_service.h"

static atomic_flag displayLocked = ATOMIC_FLAG_INIT;

static DisplaySrv::Surface surface;

static void displayLock(void) {
    while(atomic_flag_test_and_set_explicit(&displayLocked, memory_order_acquire))
        FlintAPI::Thread::yield();
}

static void displayUnlock(void) {
    atomic_flag_clear_explicit(&displayLocked, memory_order_release);
}

static void showLogo(void) {
    const HAL::Display *disp = HAL::Devices::display();
    if(disp == NULL) return;

    if(flintosLogo.format == IMG_RGB565) {
        uint16_t x = (DISPLAY_WIDTH - flintosLogo.width) / 2;
        uint16_t y = (DISPLAY_HEIGHT - flintosLogo.height) / 2;
        disp->write(x, y, flintosLogo.width, flintosLogo.height, (uint8_t *)flintosLogo.data, flintosLogo.width);
    }
}

static bool displayFlush(void) {
    const HAL::Display *disp = HAL::Devices::display();
    if(disp == NULL || surface.buffer == NULL) return false;

    displayLock();
    DisplaySrv::Surface surf = surface;
    surface.buffer = NULL;
    displayUnlock();

    if(surf.buffer == NULL) return false;

    disp->write(
        surf.invalid.x, surf.invalid.y,
        surf.invalid.width, surf.invalid.height,
        &surf.buffer[(surf.invalid.y * surf.width + surf.invalid.x) << 1], surf.width
    );

    return true;
}

void DisplaySrv::mainTask(void) {
    static const FEvent monitorEvent = {.type = 3, .data = {0}};
    static const uint32_t screenPeriodic = (1000 + DISPLAY_FREQ / 2) / DISPLAY_FREQ;
    showLogo();
    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        displayFlush();
        FlintOS::postEvent(&monitorEvent);
        int32_t remaining = screenPeriodic - (uint32_t)((uint32_t)FlintAPI::System::getTimeMillis() - tick);
        if(remaining > 0)
            FlintAPI::Thread::sleep(remaining);
    }
}

void DisplaySrv::setBrightness(uint8_t value) {
    const HAL::Display *disp = HAL::Devices::display();
    if(disp != NULL)
        disp->brightness(value);
}

void DisplaySrv::present(Surface *surf) {
    displayLock();
    if(surface.buffer != surf->buffer || surface.width != surf->width) {
        int32_t x1 = std::max<int32_t>(0, surf->invalid.x);
        int32_t y1 = std::max<int32_t>(0, surf->invalid.y);
        int32_t x2 = std::min<int32_t>(surf->invalid.x + surf->invalid.width, std::min<int32_t>(surf->width, DISPLAY_WIDTH));
        int32_t y2 = std::min<int32_t>(surf->invalid.y + surf->invalid.height, std::min<int32_t>(surf->height, DISPLAY_HEIGHT));

        if(x1 < x2 && y1 < y2) {
            surface.invalid.x = x1;
            surface.invalid.y = y1;
            surface.invalid.width = x2 - x1;
            surface.invalid.height = y2 - y1;
            surface.width = surf->width;
            surface.height = surf->height;
            surface.buffer = surf->buffer;
        }
    }
    else {
        int32_t x1 = std::max<int32_t>(0, std::min(surface.invalid.x, surf->invalid.x));
        int32_t y1 = std::max<int32_t>(0, std::min(surface.invalid.y, surf->invalid.y));
        int32_t x2 = std::max<int32_t>(surface.invalid.x + surface.invalid.width, surf->invalid.x + surf->invalid.width);
        int32_t y2 = std::max<int32_t>(surface.invalid.x + surface.invalid.height, surf->invalid.y + surf->invalid.height);
        x2 = std::min<int32_t>(x2, std::min<int32_t>(surf->width, DISPLAY_WIDTH));
        y2 = std::min<int32_t>(y2, std::min<int32_t>(surf->height, DISPLAY_HEIGHT));

        if(x1 < x2 && y1 < y2) {
            surface.invalid.x = x1;
            surface.invalid.y = y1;
            surface.invalid.width = x2 - x1;
            surface.invalid.height = y2 - y1;
            surface.height = surf->height;
        }
    }
    displayUnlock();
}
