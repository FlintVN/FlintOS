
#include <stddef.h>
#include <string.h>
#include <stdatomic.h>
#include "flintos_logo.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flint_file_reader.h"
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

void DisplaySrv::setBrightness(uint8_t value) {
    FDev::Display::brightness(value);
}

void DisplaySrv::present(Surface *surf) {
    displayLock();
    if(surface.buffer != surf->buffer || surface.width != surf->width)
        surface = *surf;
    else {
        uint16_t x2 = surf->invalid.x + surf->invalid.width;
        uint16_t y2 = surf->invalid.y + surf->invalid.height;
        if(surface.invalid.x > surf->invalid.x) surface.invalid.x = surf->invalid.x;
        if(surface.invalid.y > surf->invalid.y) surface.invalid.y = surf->invalid.y;
        if((surface.invalid.x + surface.invalid.width) < x2) surface.invalid.width = x2 - surface.invalid.x;
        if((surface.invalid.y + surface.invalid.height) < x2) surface.invalid.height = y2 - surface.invalid.y;
        surface.width = surf->width;
        surface.height = surf->height;
        surface.buffer = surf->buffer;
    }
    displayUnlock();
}

void DisplaySrv::flush(void) {
    if(surface.buffer == NULL) return;

    displayLock();
    DisplaySrv::Surface surf = surface;
    surface.buffer = NULL;
    displayUnlock();

    if(surf.buffer == NULL) return;

    FDev::Display::write(
        surf.invalid.x, surf.invalid.y,
        surf.invalid.width, surf.invalid.height,
        &surf.buffer[surf.invalid.x << 1], surf.width
    );
}

void DisplaySrv::showLogo(void) {
    if(flintosLogo.format == IMG_RGB565) {
        uint16_t x = (DISPLAY_WIDTH - flintosLogo.width) / 2;
        uint16_t y = (DISPLAY_HEIGHT - flintosLogo.height) / 2;
        FDev::Display::write(x, y, flintosLogo.width, flintosLogo.height, (uint8_t *)flintosLogo.data, flintosLogo.width);
    }
}
