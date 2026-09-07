
#ifndef __FLINTOS_EVENT_QUEUE_H
#define __FLINTOS_EVENT_QUEUE_H

#include "flint.h"

typedef struct {
    uint8_t type;
    uint32_t data[4];
} FEvent;

class FEventQueue {
private:
    FMutex eventLock;
    FExec *owner;
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    FEvent events[24];

    FEventQueue(const FEventQueue &) = delete;
    void operator=(const FEventQueue &) = delete;
public:
    FEventQueue(void);

    bool postEvent(const FEvent *event);
    const FEvent *waitEvent(FExec *ctx, uint64_t millis);
};

#endif /* __FLINTOS_EVENT_QUEUE_H */
