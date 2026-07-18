
#include <ctype.h>
#include <string.h>
#include "flint.h"
#include "flintos.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"
#include "flintos_display_service.h"
#include "flint_zip_file_reader.h"

class FlintNode : public ListNode, public Flint {
public:
    FlintNode() : ListNode(), Flint() {

    }
};

static FList<FlintNode> flints;
static FMutex fosMutex;

static void DestroyFlint(Flint *flint) {
    fosMutex.lock();
    flints.remove((FlintNode *)flint);
    fosMutex.unlock();
    flint->freeAll();
    FosDbg::getInstance()->setTarget(NULL);
    FlintAPI::System::free((FlintNode *)flint);
}

static void FlintTerminated(Flint *flint) {
    DestroyFlint(flint);
}

char *Trim(char *text) {
    if(text == NULL) return NULL;
    while(isspace((unsigned char)*text)) text++;

    size_t length = strlen(text);
    if(length == 0) return text;
    char *end = text + length - 1;
    while(end > text && isspace((unsigned char)*end)) {
        *end = 0;
        end--;
    }
    return text;
}

struct AppManifest {
    char mainClass[FILE_NAME_BUFF_SIZE];
    char midletClass[FILE_NAME_BUFF_SIZE];
};

static bool CopyClassName(char *destination, size_t size, const char *source) {
    source = Trim((char *)source);
    size_t length = strnlen(source, size);
    if(length == 0 || length >= size) return false;
    memcpy(destination, source, length + 1);
    for(size_t i = 0; i < length; i++) {
        if(destination[i] == '.') destination[i] = '/';
    }
    return true;
}

static bool ReadAppManifest(const char *jarPath, AppManifest *manifest) {
    char line[FILE_NAME_BUFF_SIZE];
    ZipFileReader zip(NULL, jarPath);
    if(!zip.open()) return false;
    if(!zip.gotoFile("META-INF/MANIFEST.MF")) {
        zip.close();
        return false;
    }

    bool valid = true;
    while(true) {
        int32_t count = zip.readLine(line, sizeof(line));
        if(count == -1) {
            valid = false;
            break;
        }
        if(count < 2) break;

        char *text = Trim(line);
        if(strncmp(text, "Main-Class:", 11) == 0) {
            if(!CopyClassName(manifest->mainClass, sizeof(manifest->mainClass), text + 11)) {
                valid = false;
                break;
            }
        }
        else if(strncmp(text, "MIDlet-1:", 9) == 0) {
            char *value = text + 9;
            char *firstComma = strchr(value, ',');
            char *secondComma = firstComma == NULL ? NULL : strchr(firstComma + 1, ',');
            if(secondComma == NULL || !CopyClassName(
                    manifest->midletClass,
                    sizeof(manifest->midletClass),
                    secondComma + 1)) {
                valid = false;
                break;
            }
        }
    }
    zip.close();
    return valid;
}

static bool StartApplication(Flint *flint, const char *jarPath) {
    AppManifest manifest = {};
    if(!ReadAppManifest(jarPath, &manifest)) return false;
    if(manifest.midletClass[0] != 0) {
        return flint->start(
            "javax/microedition/midlet/MIDletLifecycle",
            manifest.midletClass
        );
    }
    return flint->start(manifest.mainClass[0] == 0 ? "Main" : manifest.mainClass);
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
            if(!flint->setProgram(text) || !StartApplication(flint, text)) {
                DestroyFlint(flint);
            }
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
