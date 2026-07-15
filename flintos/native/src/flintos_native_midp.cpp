
#include <stddef.h>
#include "flint_array_object.h"
#include "flint.h"
#include "flint_execution.h"
#include "flint_system_api.h"
#include "flintos_default_conf.h"
#include "flintos_display_service.h"
#include "flintos_native_midp.h"

static void ThrowNullPointer(FNIEnv *env, const char *message) {
    env->throwNew(env->findClass("java/lang/NullPointerException"), message);
}

static void ThrowIllegalArgument(FNIEnv *env, const char *message) {
    env->throwNew(env->findClass("java/lang/IllegalArgumentException"), message);
}

jvoid NativeMidpLcd_Init(FNIEnv *env) {
    (void)env;
    /* FlintOS initializes the display before it starts a MIDlet. */
}

jint NativeMidpLcd_Width(FNIEnv *env) {
    (void)env;
    return DISPLAY_WIDTH;
}

jint NativeMidpLcd_Height(FNIEnv *env) {
    (void)env;
    return DISPLAY_HEIGHT;
}

jvoid NativeMidpLcd_Present(FNIEnv *env, jbyteArray frameBuffer) {
    if(frameBuffer == NULL) {
        ThrowNullPointer(env, "frameBuffer");
        return;
    }

    constexpr uint32_t requiredLength = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2;
    if(frameBuffer->getLength() != requiredLength) {
        ThrowIllegalArgument(env, "Invalid frame buffer length");
        return;
    }

    DisplaySrv::write(
        0,
        0,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        (uint8_t *)frameBuffer->getData()
    );
}

jint NativeMidpLcd_ReadKey(FNIEnv *env) {
    (void)env;
    return -1;
}

jvoid NativeMidpTouch_Init(FNIEnv *env) {
    (void)env;
}

jint NativeMidpTouch_Poll(FNIEnv *env) {
    (void)env;
    return -1;
}

jvoid NativeMidpTouch_Boost(FNIEnv *env) {
    (void)env;
    FlintAPI::Thread::yield();
}

jstring NativeMidpResourceLoader_GetProgramPath(FNIEnv *env) {
    const char *programPath = ((FExec *)env)->getFlint()->getProgram();
    if(programPath == NULL)
        return NULL;
    return env->newString("%s", programPath);
}
