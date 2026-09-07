
#ifndef __FLINTOS_PROCESS_H
#define __FLINTOS_PROCESS_H

#include "flint.h"
#include "flintos_event_queue.h"

class FProcess : public ListNode, public Flint {
private:
    FEventQueue eventQueue;

    FProcess(const FProcess &) = delete;
    void operator=(const FProcess &) = delete;
public:
    FProcess(void);

    FEventQueue *getEventQueue(void);
};

#endif /* __FLINTOS_PROCESS_H */
