
#include "flint_array_object.h"
#include "flintos_default_conf.h"
#include "flintos_native_display.h"
#include "flintos_display_service.h"

jint NativeDisplay_GetWidth(FNIEnv *env) {
    (void)env;
    return DISPLAY_WIDTH;
}

jint NativeDisplay_GetHeight(FNIEnv *env) {
    (void)env;
    return DISPLAY_HEIGHT;
}

jvoid NativeDisplay_Write(FNIEnv *env, jint x, jint y, jint w, jint h, jbyteArray data) {
    if(x < 0 || y < 0 || (x + w) > DISPLAY_WIDTH || (y + h) > DISPLAY_HEIGHT) {
        env->throwNew(env->findClass("java/lang/IllegalArgumentException"), "Writing area extends beyond the screen");
        return;
    }
    DisplaySrv::write(x, y, w, h, (uint8_t *)data->getData());
}

jvoid NativeDisplay_SetBrightness(FNIEnv *env, jint value) {
    if(value < 0) value = 0;
    else if(value > 100) value = 100;
    DisplaySrv::setBrightness(value);
}
