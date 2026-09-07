
#include "flintos_event_queue.h"

FEventQueue::FEventQueue(void) : eventLock() {
    owner = NULL;
    head = 0;
    tail = 0;
    count = 0;
}

bool FEventQueue::postEvent(const FEvent *event) {
    if(count == LENGTH(events)) return false;

    eventLock.lock();
    if(count < LENGTH(events)) {
        events[head] = *event;
        head = (head + 1) % LENGTH(events);
        count++;
    }
    else {
        eventLock.unlock();
        return false;
    }
    if(owner != NULL)
        FlintAPI::Thread::notify(owner->getOwnerThread()->getHandle(), FlintAPI::Thread::THREAD_NOTIFY_SYSTEM_EVENT);
    eventLock.unlock();
    return true;
}

const FEvent * FEventQueue::waitEvent(FExec *ctx, uint64_t millis) {
    int64_t startTime = FlintAPI::System::getTimeMillis();
    int64_t waitTime = 0;
    if(ctx == NULL) return NULL;
    jthread ownerThread = ctx->getOwnerThread();

    if(ownerThread->getHandle() != FlintAPI::Thread::getCurrentThread()) {
        ctx->throwNew(ctx->findClass("java/lang/IllegalMonitorStateException"), "current native thread is not owner");
        return NULL;
    }

    eventLock.lock();
    if(owner == NULL)
        owner = ctx;
    else {
        if(owner != ctx) {
            eventLock.unlock();
            ctx->throwNew(ctx->findClass("java/lang/IllegalMonitorStateException"), "current java thread is not owner");
            return NULL;
        }
    }
    eventLock.unlock();

    while(true) {
        if(ctx->hasTerminateRequest() || ownerThread->getInterrupt()) {
            owner = NULL;
            return NULL;
        }

        if(count > 0) {
            eventLock.lock();
            FEvent *event = &events[tail];
            tail = (tail + 1) % LENGTH(events);
            count--;
            eventLock.unlock();
            owner = NULL;
            return event;
        }

        if(millis > 0) {
            waitTime = millis - (FlintAPI::System::getTimeMillis() - startTime);
            if(waitTime > 1000)
                waitTime = 1000;
            else if(waitTime <= 0) {
                owner = NULL;
                return NULL;
            }
        }
        FlintAPI::Thread::wait((uint32_t)waitTime);
    }
}
