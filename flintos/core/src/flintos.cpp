
#include "flint.h"
#include "flintos.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"
#include "flintos_display_service.h"

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

char *Trim(char *text) {
    if(text == NULL) return NULL;
    while(isspace(*text)) text++;

    char *end = text + strlen(text) - 1;
    while(end > text && isspace(*end)) {
        *end = 0;
        end--;
    }
    return text;
}

void FlintOS::startup() {
    char path[FILE_NAME_BUFF_SIZE];

    FileReader reader(NULL, "/sys/startup.ini");
    if(!reader.open()) return;

    while(reader.readLine(path, sizeof(path)) != -1) {
        char *text = Trim(path);
        if(FlintAPI::IO::finfo(text, NULL) == FlintAPI::IO::FILE_RESULT_OK) {
            Flint *flint = FlintOS::newFlint();
            if(flint == NULL) {
                reader.close();
                return;
            }
            flint->setProgram(text);
            flint->start();
        }
    }
    reader.close();
}

static void DebuggerTask() {
    FosDbg::getInstance()->receiveTask();
}

void FlintOS::main(void) {
    FDev::Display::init();
    FDev::Display::brightness(100);
    DisplaySrv::showLogo();
    FDev::Audio::init();
    AudioSrv::setVolumn(100);
    FDev::WiFi::init();
    FlintOS::startup();
    FlintAPI::Thread::create((void (*)(void *))AudioSrv::mainTask, NULL, 1024);
    FlintAPI::Thread::create((void (*)(void *))DebuggerTask, NULL, 6144);

    uint32_t notifiValue;

    uint32_t screenPeriodic = (1000 + DISPLAY_FREQ / 2) / DISPLAY_FREQ;
    uint32_t screenStart = (uint32_t)FlintAPI::System::getTimeMillis();
    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        if((uint32_t)(tick - screenStart) >= screenPeriodic) {
            DisplaySrv::update();
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
