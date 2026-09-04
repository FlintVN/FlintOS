
#include "flint.h"
#include "flintos.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"
#include "flint_zip_file_reader.h"
#include "flintos_display_service.h"

static FProcess *homeApp = NULL;
static FProcess *allowForeground = NULL;
static FProcess *currentForeground = NULL;

FProcess::FProcess(void) : ListNode(), Flint() {

}

static FList<FProcess> processList;
static FMutex fosMutex;

static void flintTerminated(Flint *flint) {
    FProcess *process = (FProcess *)flint;
    if(process == homeApp)
        homeApp = NULL;
    if(process == allowForeground)
        allowForeground = homeApp;
    if(process == currentForeground)
        currentForeground = homeApp;
    fosMutex.lock();
    processList.remove(process);
    fosMutex.unlock();
    process->freeAll();
    FosDbg::getInstance()->setTarget(NULL);
    FlintAPI::System::free(process);
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
    bool isFirst = true;
    char path[FILE_NAME_BUFF_SIZE];

    FileReader reader(NULL, "/sys/startup.ini");
    if(!reader.open()) return;

    while(reader.readLine(path, sizeof(path)) != -1) {
        char *text = trim(path);
        if(FlintAPI::IO::finfo(text, NULL) == FlintAPI::IO::FILE_RESULT_OK) {
            FProcess *process = FlintOS::open(text);
            if(isFirst) {
                setHomeApp(process);
                isFirst = false;
            }
        }
    }
    reader.close();
}

static void debuggerTask() {
    FosDbg::getInstance()->receiveTask();
}

void FlintOS::main(void) {
    FDev::Display::init();
    DisplaySrv::showLogo();
    FDev::Display::brightness(100);
    FDev::Audio::init();
    AudioSrv::setVolumn(100);
    FDev::WiFi::init();
    FlintOS::startup();
    FlintAPI::Thread::create((void (*)(void *))AudioSrv::mainTask, NULL, 512);
    FlintAPI::Thread::create((void (*)(void *))debuggerTask, NULL, 6144);

    uint32_t notifiValue;

    uint32_t screenPeriodic = (1000 + DISPLAY_FREQ / 2) / DISPLAY_FREQ;
    uint32_t screenStart = (uint32_t)FlintAPI::System::getTimeMillis();
    while(true) {
        uint32_t tick = (uint32_t)FlintAPI::System::getTimeMillis();
        if((uint32_t)(tick - screenStart) >= screenPeriodic) {
            DisplaySrv::flush();
            screenStart = tick;
        }
        if(FlintAPI::Thread::wait(2, &notifiValue)) {
            // TODO
        }
    }
}

FProcess *FlintOS::newProcess(void) {
    FProcess *process = (FProcess *)FlintAPI::System::malloc(sizeof(FProcess));
    if(process == NULL) return NULL;
    new (process)FProcess();
    process->terminatedCallback(flintTerminated);
    fosMutex.lock();
    processList.add(process);
    fosMutex.unlock();
    return process;
}

typedef struct {
    uint8_t type;
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
            manifest->mainCls = flint->getUtf8(NULL, mainCls, len);

            if(manifest->mainCls == NULL) goto exit;
        }
        else if(strncmp(line, "MIDlet-1:", 9) == 0) {
            manifest->type = 1;

            char *name = getNextValue(&line[9], ',', &len);
            char *icon = getNextValue(&name[len], ',', &len);
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

static FProcess *runApplication(const char *file) {
    FProcess *process = FlintOS::newProcess();
    if(process == NULL) return NULL;

    do {
        Manifest manifest;
        if(!readManifest(process, file, &manifest)) break;
        if(!process->setProgram(file)) break;

        if(manifest.type == 0) {    /* Normal application */
            if(!process->startToMain()) break;
        }
        else {                      /* J2ME application */
            static constexpr ConstNameAndType startAppName("startApp", "(Ljava/lang/Class;)V");

            JClass *mainCls = process->findClass(NULL, manifest.mainCls);
            if(mainCls == NULL) break;

            JClass *ams = process->findClass(NULL, "flintos/midp/AMS");
            if(ams == NULL) break;

            MethodInfo *method = process->findMethod(NULL, ams, (ConstNameAndType *)&startAppName);
            if(method == NULL) break;

            if(!process->start(method, 1, mainCls)) break;
        }

        FlintOS::setForeground(process);
        return process;
    } while(0);

    flintTerminated(process);
    return NULL;
}

static const char *getExtensionName(const char *fileName) {
    int32_t endIdx = strlen(fileName) - 1;
    while(endIdx >= 0 && fileName[endIdx] != '.') endIdx--;
    return (endIdx >= 0) ? &fileName[endIdx] : NULL;
}

FProcess *FlintOS::open(const char *file) {
    if(file == NULL) return NULL;
    const char *extName = getExtensionName(file);
    if(strcasecmp(extName, ".jar") == 0)
        return runApplication(file);
    return NULL;
}

void FlintOS::setHomeApp(FProcess *process) {
    homeApp = process;
}

bool FlintOS::isForeground(FProcess *process, bool checkOnly) {
    if(process == currentForeground)
        return true;
    if(process == allowForeground) {
        if(!checkOnly)
            currentForeground = allowForeground;
        return true;
    }
    return false;
}

void FlintOS::setForeground(FProcess *process) {
    allowForeground = process;
}
