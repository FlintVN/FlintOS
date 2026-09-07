
#include <string.h>
#include "flintos_process.h"
#include "flint_java_object.h"
#include "flint_array_object.h"
#include "flintos_event_queue.h"
#include "flintos_native_native_event_receiver.h"

class JNativeEvent : public JObject {
public:
    void setType(int32_t type) { getFieldByIndex(0)->setInt32(type); }

    int32_t *getData(void) { return ((jintArray)getFieldByIndex(1)->getObj())->getData(); }
};

jbool NativeInputEvent_WaitEvent1(FNIEnv *env, jobject event) {
    FExec *ctx = (FExec *)env;
    FEventQueue *eventQueue = ((FProcess *)(ctx->getFlint()))->getEventQueue();
    const FEvent *fevent = eventQueue->waitEvent(ctx, 0);

    if(fevent == NULL) return false;

    JNativeEvent *nativeEvent = (JNativeEvent *)event;
    nativeEvent->setType(fevent->type);
    memcpy(nativeEvent->getData(), fevent->data, sizeof(fevent->data));

    return true;
}

jbool NativeInputEvent_WaitEvent2(FNIEnv *env, jobject event, jlong millis) {
    FExec *ctx = (FExec *)env;
    FEventQueue *eventQueue = ((FProcess *)(ctx->getFlint()))->getEventQueue();
    const FEvent *fevent = eventQueue->waitEvent(ctx, millis);

    if(fevent == NULL) return false;

    JNativeEvent *nativeEvent = (JNativeEvent *)event;
    nativeEvent->setType(fevent->type);
    memcpy(nativeEvent->getData(), fevent->data, sizeof(fevent->data));

    return true;
}
