
#include "flint.h"
#include "flintos.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"
#include "flint_zip_file_reader.h"
#include "flintos_display_service.h"

class FlintNode : public ListNode, public Flint {
public:
    FlintNode() : ListNode(), Flint() {

    }
};

static FList<FlintNode> flints;
static FMutex fosMutex;

static void flintTerminated(Flint *flint) {
    fosMutex.lock();
    flints.remove((FlintNode *)flint);
    fosMutex.unlock();
    flint->freeAll();
    FosDbg::getInstance()->setTarget(NULL);
    FlintAPI::System::free((FlintNode *)flint);
}

char *trim(char *text) {
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
        char *text = trim(path);
        if(FlintAPI::IO::finfo(text, NULL) == FlintAPI::IO::FILE_RESULT_OK)
            FlintOS::open(text);
    }
    reader.close();
}

static void debuggerTask() {
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
    FlintAPI::Thread::create((void (*)(void *))debuggerTask, NULL, 6144);

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
    flint->terminatedCallback(flintTerminated);
    fosMutex.lock();
    flints.add(flint);
    fosMutex.unlock();
    return (Flint *)flint;
}

typedef struct {
    uint8_t type;
    const char *name;
    const char *icon;
    const char *mainCls;
} Manifest;

static bool readManifest(Flint *flint, const char *jarPath, Manifest *manifest) {
    extern char *getNextValue(char *value, char seperator, uint32_t *len);
    uint32_t len;
    bool ret = false;
    char buff[FILE_NAME_BUFF_SIZE];
    ZipFileReader zip(NULL, jarPath);
    if(!zip.open()) return false;
    if(!zip.gotoFile("META-INF/MANIFEST.MF")) goto exit;

    while(true) {
        int32_t br = zip.readLine(buff, FILE_NAME_BUFF_SIZE);
        if(br == -1) goto exit;
        if(br < 2) break;

        char *line = trim(buff);
        if(strncmp(line, "Main-Class:", 11) == 0) {
            char *mainCls = getNextValue(&line[11], ',', &len);
            if(len == 0) goto exit;
            for(uint32_t i = 0; i < len; i++)
                if(mainCls[i] == '.') mainCls[i] = '/';

            manifest->type = 0;
            manifest->name = NULL;
            manifest->icon = NULL;
            manifest->mainCls = flint->getUtf8(NULL, mainCls, len);

            if(manifest->mainCls == NULL) goto exit;
        }
        else if(strncmp(line, "MIDlet-1:", 9) == 0) {
            manifest->type = 1;

            char *name = getNextValue(&line[9], ',', &len);
            if(len > 0) {
                manifest->name = flint->getUtf8(NULL, name, len);
                if(manifest->name == NULL) goto exit;
            }
            else
                manifest->name = NULL;

            char *icon = getNextValue(&name[len], ',', &len);
            if(len > 0) {
                manifest->icon = flint->getUtf8(NULL, icon, len);
                if(manifest->icon == NULL) goto exit;
            }
            else
                manifest->icon = NULL;

            char *mainCls = getNextValue(&icon[len], ',', &len);
            if(len > 0) {
                manifest->mainCls = flint->getUtf8(NULL, mainCls, len);
                if(manifest->mainCls == NULL) goto exit;
            }
            else
                manifest->mainCls = NULL;
        }
    }
    ret = true;
exit:
    zip.close();
    return ret;
}

static bool runApplication(const char *file) {
    Flint *flint = FlintOS::newFlint();
    if(flint == NULL) return false;

    do {
        Manifest manifest;
        if(!readManifest(flint, file, &manifest)) break;
        if(!flint->setProgram(file)) break;

        if(manifest.type == 0) {    /* Normal application */
            if(!flint->startToMain()) break;
        }
        else {                      /* J2ME application */
            static constexpr ConstNameAndType startAppName("startApp", "()V");

            JClass *mainCls = flint->findClass(NULL, manifest.mainCls);
            if(mainCls == NULL) break;

            MethodInfo *method = flint->findMethod(NULL, mainCls, (ConstNameAndType *)&startAppName);
            if(method == NULL) break;

            if(!flint->start(method)) break;
        }

        return true;
    } while(0);

    flintTerminated(flint);
    return false;
}

static const char *getExtensionName(const char *fileName) {
    int32_t endIdx = strlen(fileName) - 1;
    while(endIdx >= 0 && fileName[endIdx] != '.') endIdx--;
    return (endIdx >= 0) ? &fileName[endIdx] : NULL;
}

bool FlintOS::open(const char *file) {
    if(file == NULL) return false;
    const char *extName = getExtensionName(file);
    if(strcasecmp(extName, ".jar") == 0)
        return runApplication(file);
    return false;
}
