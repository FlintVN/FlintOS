
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "flint_system_api.h"

using namespace FlintAPI::Thread;

ThreadHandle FlintAPI::Thread::create(void (*task)(void *), void *param, uint32_t stackSize) {
    TaskHandle_t xHandle = NULL;
    uint32_t nativeStack = (stackSize > 6144) ? stackSize : 6144;
    if(xTaskCreateWithCaps(task, "FlintJavaThread", nativeStack, param, tskIDLE_PRIORITY + 1, &xHandle, MALLOC_CAP_SPIRAM) != pdPASS)
        return NULL;
    return (void *)xHandle;
}

ThreadHandle FlintAPI::Thread::getCurrentThread(void) {
    return (void *)xTaskGetCurrentTaskHandle();
}

void FlintAPI::Thread::terminate(ThreadHandle handle) {
    vTaskDeleteWithCaps((TaskHandle_t)handle);
}

void FlintAPI::Thread::sleep(uint32_t ms) {
    TickType_t tick = pdMS_TO_TICKS(ms);
    vTaskDelay((tick < 1) ? 1 : tick);
}

void FlintAPI::Thread::yield(void) {
    vTaskDelay(1);
}

bool FlintAPI::Thread::wait(uint32_t ms, uint32_t *notifyValue) {
    if(ms > 0)
        return xTaskNotifyWait(0, ULONG_MAX, notifyValue, pdMS_TO_TICKS(ms));
    else
        return xTaskNotifyWait(0, ULONG_MAX, notifyValue, portMAX_DELAY);
}

void FlintAPI::Thread::notify(ThreadHandle handle, uint32_t notifyValue) {
    xTaskNotify((TaskHandle_t)handle, notifyValue, eSetValueWithOverwrite);
}
