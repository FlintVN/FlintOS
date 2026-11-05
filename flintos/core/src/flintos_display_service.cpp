
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

#define MAX_SURFACE_QUEUE           3

static atomic<bool> displayLocked = ATOMIC_FLAG_INIT;
static uint8_t brightness = 0;

static uint8_t surfaceHead = 0;
static uint8_t surfaceTail = 0;
static uint8_t surfaceCount = 0;
static DisplaySrv::Surface surfaces[MAX_SURFACE_QUEUE];

static void displayLock(void) {
    while (displayLocked.exchange(true, memory_order_acquire))
        FlintAPI::Thread::yield();
}

static void displayUnlock(void) {
    displayLocked.exchange(false, memory_order_release);
}

static void showLogo(void) {
    const HAL::Display *disp = HAL::Devices::display();
    if (disp == NULL) return;

    uint16_t x = (DISPLAY_WIDTH - flintosLogo.width) / 2;
    uint16_t y = (DISPLAY_HEIGHT - flintosLogo.height) / 2;
    disp->write(x, y, flintosLogo.width, flintosLogo.height, (uint8_t *)flintosLogo.data, flintosLogo.width);
}

static void displayFlush(void) {
    const HAL::Display *disp = HAL::Devices::display();
    if (disp == NULL || surfaceCount == 0) return;

    displayLock();
    DisplaySrv::Surface surf = surfaces[surfaceTail];
    surfaceTail = (surfaceTail + 1) % LENGTH(surfaces);
    surfaceCount--;
    displayUnlock();

    disp->write(
        surf.invalid.x, surf.invalid.y,
        surf.invalid.width, surf.invalid.height,
        &surf.buffer[(surf.invalid.y * surf.width + surf.invalid.x) << 1], surf.width
    );
}

void DisplaySrv::mainTask(void) {
    static const FEvent monitorEvent = {.type = MONITOR_EVENT, .data = {0}};
    static const uint32_t screenPeriodic = (1000 + DISPLAY_FREQ / 2) / DISPLAY_FREQ;
    showLogo();
    while (true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        FlintOS::postEvent(&monitorEvent);
        displayFlush();
        int32_t remaining = screenPeriodic - (uint32_t)((uint32_t)FlintAPI::System::getTimeMillis() - tick);
        if (remaining > 0)
            FlintAPI::Thread::sleep(remaining);
    }
}

uint8_t DisplaySrv::getBrightness(void) {
    return brightness;
}

void DisplaySrv::setBrightness(uint8_t value) {
    const HAL::Display *disp = HAL::Devices::display();
    brightness = value;
    if (disp != NULL)
        disp->brightness(value);
}

static bool setSurface(DisplaySrv::Surface *surf, DisplaySrv::Surface *value) {
    int32_t x1 = std::max<int32_t>(0, value->invalid.x);
    int32_t y1 = std::max<int32_t>(0, value->invalid.y);
    int32_t x2 = std::min<int32_t>(value->invalid.x + value->invalid.width, std::min<int32_t>(value->width, DISPLAY_WIDTH));
    int32_t y2 = std::min<int32_t>(value->invalid.y + value->invalid.height, std::min<int32_t>(value->height, DISPLAY_HEIGHT));

    if (x1 < x2 && y1 < y2) {
        surf->invalid.x = x1;
        surf->invalid.y = y1;
        surf->invalid.width = x2 - x1;
        surf->invalid.height = y2 - y1;
        surf->width = value->width;
        surf->height = value->height;
        surf->buffer = value->buffer;
        return true;
    }

    return false;
}

static void mergeSurface(DisplaySrv::Surface *surf, DisplaySrv::Surface *value) {
    int32_t x1 = std::max<int32_t>(0, std::min(surf->invalid.x, value->invalid.x));
    int32_t y1 = std::max<int32_t>(0, std::min(surf->invalid.y, value->invalid.y));
    int32_t x2 = std::max<int32_t>(surf->invalid.x + surf->invalid.width, value->invalid.x + value->invalid.width);
    int32_t y2 = std::max<int32_t>(surf->invalid.y + surf->invalid.height, value->invalid.y + value->invalid.height);
    x2 = std::min<int32_t>(x2, std::min<int32_t>(value->width, DISPLAY_WIDTH));
    y2 = std::min<int32_t>(y2, std::min<int32_t>(value->height, DISPLAY_HEIGHT));

    if (x1 < x2 && y1 < y2) {
        surf->invalid.x = x1;
        surf->invalid.y = y1;
        surf->invalid.width = x2 - x1;
        surf->invalid.height = y2 - y1;
        surf->height = value->height;
    }
}

static bool contains(DisplaySrv::Surface *a, DisplaySrv::Surface *b) {
    if (
        a->invalid.x <= b->invalid.x &&
        a->invalid.y <= b->invalid.y &&
        a->invalid.x + a->invalid.width >= b->invalid.x + b->invalid.width &&
        a->invalid.y + a->invalid.height >= b->invalid.y + b->invalid.height
    ) {
        return true;
    }
    return false;
}

void DisplaySrv::present(Surface *surf) {
    displayLock();
    if (surfaceCount > 0) {
        uint8_t surfaceLatest = (surfaceHead + (LENGTH(surfaces) - 1)) % LENGTH(surfaces);
        DisplaySrv::Surface *surface = &surfaces[surfaceLatest];
        if (surface->buffer == surf->buffer) {
            if (surface->width != surf->width)
                setSurface(surface, surf);
            else
                mergeSurface(surface, surf);
            displayUnlock();
            return;
        }
        else if (contains(surf, surface)) {
            setSurface(surface, surf);
            displayUnlock();
            return;
        }
    }
    if (setSurface(&surfaces[surfaceHead], surf)) {
        surfaceHead = (surfaceHead + 1) % LENGTH(surfaces);
        if (surfaceCount < LENGTH(surfaces))
            surfaceCount++;
    }
    displayUnlock();
}
