
#include "flintos_process.h"
#include "flint_system_api.h"

FProcess::FProcess(void) : ListNode(), Flint(), eventQueue() {

}

FEventQueue *FProcess::getEventQueue(void) {
    return &eventQueue;
}
