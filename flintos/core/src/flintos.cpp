
#include "flint.h"
#include "flintos.h"
#include "flintos_display.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_default_conf.h"

class FlintNode : public ListNode, public Flint {
public:
    FlintNode() : ListNode(), Flint() {

    }
};

static FList<FlintNode> flints;
static FMutex fosMutex;

static void FlintTerminated(Flint *flint) {
    fosMutex.lock();
    flints.remove((FlintNode *)flint);
    fosMutex.unlock();
    flint->freeAll();
    FosDbg::getInstance()->setTarget(NULL);
    FlintAPI::System::free((FlintNode *)flint);
}

static void DebuggerTask() {
    FosDbg::getInstance()->receiveTask();
}

void FlintOS::main(void) {
    FosDisplay::setBrightness(100);
    FosDisplay::showLogo();
    FlintAPI::Thread::create((void (*)(void *))DebuggerTask, NULL, 4096);

    uint32_t notifiValue;

    uint32_t screenPeriodic = (1000 + DISPLAY_FREQ / 2) / DISPLAY_FREQ;
    uint32_t screenStart = (uint32_t)FlintAPI::System::getTimeMillis();
    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        if((uint32_t)(tick - screenStart) >= screenPeriodic) {
            if(FosDisplay::update())
                screenStart = tick;
        }
        if(FlintAPI::Thread::wait(2, &notifiValue)) {
            // TODO
        }
    }
}

Flint *FlintOS::newFlint(void) {
    FlintNode *flint = (FlintNode *)FlintAPI::System::malloc(sizeof(FlintNode));
    if(flint == NULL) return NULL;
    new (flint)FlintNode();
    flint->terminatedCallback(FlintTerminated);
    fosMutex.lock();
    flints.add(flint);
    fosMutex.unlock();
    return (Flint *)flint;
}
