
#include "flint.h"
#include "flintos.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"

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

void FlintOS::main(void) {
    FosDbg::getInstance()->receiveTask();
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
