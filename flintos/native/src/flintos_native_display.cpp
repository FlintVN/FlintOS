
#include "flintos.h"
#include "flint_array_object.h"
#include "flintos_default_conf.h"
#include "flintos_native_display.h"
#include "flintos_display_service.h"

class JDisplay : public JObject {
public:
    jint getWidth() { return getFieldByIndex(0)->getInt32(); }
    jint getHeight() { return getFieldByIndex(1)->getInt32(); }
    jbyteArray getBuffer() { return (jbyteArray)getFieldByIndex(2)->getObj(); }
};

jint NativeDisplay_GetPrimaryWidth(FNIEnv *env) {
    (void)env;
    return DISPLAY_WIDTH;
}

jint NativeDisplay_GetPrimaryHeight(FNIEnv *env) {
    (void)env;
    return DISPLAY_HEIGHT;
}

jbool NativeDisplay_IsForeground(FNIEnv *env) {
    return FlintOS::isForeground((FProcess *)((FExec *)env)->getFlint());
}

jvoid NativeDisplay_SetBrightness(FNIEnv *env, jint value) {
    if(FlintOS::isForeground((FProcess *)((FExec *)env)->getFlint())) {
        if(value < 0) value = 0;
        else if(value > 100) value = 100;
        DisplaySrv::setBrightness(value);
    }
}

jvoid NativeDisplay_Present1(FNIEnv *env, jobject obj) {
    if(FlintOS::isForeground((FProcess *)((FExec *)env)->getFlint(), false)) {
        JDisplay *disp = (JDisplay *)obj;
        DisplaySrv::Surface surf;
        surf.invalid.x = 0;
        surf.invalid.y = 0;
        surf.invalid.width = disp->getWidth();
        surf.invalid.height = disp->getHeight();
        surf.width = disp->getWidth();
        surf.height = disp->getHeight();
        surf.buffer = (uint8_t *)disp->getBuffer()->getData();
        DisplaySrv::present(&surf);
    }
    else
        FlintAPI::Thread::sleep(1);
}

jvoid NativeDisplay_Present2(FNIEnv *env, jobject obj, jint x, jint y, jint w, jint h) {
    if(FlintOS::isForeground((FProcess *)((FExec *)env)->getFlint(), false)) {
        JDisplay *disp = (JDisplay *)obj;
        DisplaySrv::Surface surf;
        surf.invalid.x = x;
        surf.invalid.y = y;
        surf.invalid.width = w;
        surf.invalid.height = h;
        surf.width = disp->getWidth();
        surf.height = disp->getHeight();
        surf.buffer = (uint8_t *)disp->getBuffer()->getData();
        DisplaySrv::present(&surf);
    }
    else
        FlintAPI::Thread::sleep(1);
}
