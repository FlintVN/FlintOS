
#include "flintos.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_hal_devices.h"
#include "flintos_default_conf.h"
#include "flint_zip_file_reader.h"
#include "flintos_audio_service.h"
#include "flintos_input_service.h"
#include "flintos_display_service.h"
#include "flintos_zip_class_cache.h"

static FProcess *homeApp = NULL;
static FProcess *allowForeground = NULL;
static FProcess *currentForeground = NULL;

static FList<FProcess> processList;
static FMutex fosMutex;

static const FEvent foregroundEvent = {.type = FOREGROUND_EVENT, .data = {0}};
static const FEvent backgroundEvent = {.type = BACKGROUND_EVENT, .data = {0}};

static void flintTerminated(Flint *flint) {
    FProcess *process = (FProcess *)flint;
    if (process == homeApp)
        homeApp = NULL;
    if (process == allowForeground) {
        allowForeground = homeApp;
        if (homeApp != NULL)
            homeApp->getEventQueue()->postEvent(&foregroundEvent);
    }
    if (process == currentForeground) {
        currentForeground = homeApp;
        if (homeApp != NULL)
            homeApp->getEventQueue()->postEvent(&foregroundEvent);
    }
    fosMutex.lock();
    processList.remove(process);
    process->freeAll();
    FosDbg::getInstance()->removeTarget(flint);
    FlintAPI::System::free(process);
    fosMutex.unlock();
}

char *trim(char *text) {
    if (text == NULL) return NULL;
    while (isspace(*text)) text++;

    char *end = text + strlen(text) - 1;
    while (end > text && isspace(*end)) {
        *end = 0;
        end--;
    }
    return text;
}

void FlintOS::startup() {
    bool isFirst = true;
    char path[FILE_NAME_BUFF_SIZE];

    FileReader reader(NULL, "/sys/startup.ini");
    if (!reader.open()) return;

    while (reader.readLine(path, sizeof(path)) != -1) {
        char *text = trim(path);
        FProcess *process = FlintOS::open(text);
        if (isFirst) {
            if (process != NULL)
                setHomeApp(process);
            isFirst = false;
        }
    }
    reader.close();
}

static void debuggerTask() {
    FosDbg::getInstance()->receiveTask();
}

void FlintOS::main(void) {
    if (HAL::Devices::display() != NULL) {
        HAL::Devices::display()->init();
        DisplaySrv::setBrightness(100);
        FlintAPI::Thread::create((void (*)(void *))DisplaySrv::mainTask, NULL, 512, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    }
    if (HAL::Devices::touch())
        HAL::Devices::touch()->init();
    FlintAPI::Thread::create((void (*)(void *))InputSrv::mainTask, NULL, 512, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    if (HAL::Devices::audio() != NULL) {
        HAL::Devices::audio()->init();
        AudioSrv::setVolume(100);
        FlintAPI::Thread::create((void (*)(void *))AudioSrv::mainTask, NULL, 512, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    }
    if (HAL::Devices::wifi() != NULL)
        HAL::Devices::wifi()->init();
    ZipClassCache::init();
    FlintOS::startup();
    FlintAPI::Thread::create((void (*)(void *))debuggerTask, NULL, 6144, FlintAPI::Thread::THREAD_PRIORITY_MEDIUM);
}

FProcess *FlintOS::newProcess(void) {
    FProcess *process = (FProcess *)FlintAPI::System::malloc(sizeof(FProcess));
    if (process == NULL) return NULL;
    new (process)FProcess();
    process->terminatedCallback(flintTerminated);
    lock();
    processList.add(process);
    unlock();
    return process;
}

typedef struct {
    uint8_t type : 1;
    uint8_t launchMode : 1;
    const char *jarFile;
    char mainCls[FILE_NAME_BUFF_SIZE];
} Manifest;

static bool readManifest(const char *jarPath, Manifest *manifest) {
    extern char *getNextValue(char *value, char seperator, uint32_t *len);
    uint32_t len;
    bool ret = false;
    char buff[FILE_NAME_BUFF_SIZE];
    ZipFileReader zip(NULL, jarPath);
    if (!zip.open()) return false;
    if (!zip.openEntry("META-INF/MANIFEST.MF")) goto exit;

    manifest->launchMode = 0;
    manifest->jarFile = jarPath;
    manifest->mainCls[0] = 0;

    while (true) {
        int32_t br = zip.readLine(buff, FILE_NAME_BUFF_SIZE);
        if (br == -1) goto exit;
        if (br < 2) break;

        char *line = trim(buff);
        if (strncmp(line, "Main-Class:", 11) == 0) {
            char *mainCls = getNextValue(&line[11], ',', &len);
            if (len == 0 || len >= sizeof(manifest->mainCls)) goto exit;
            for (uint32_t i = 0; i < len; i++) if (mainCls[i] == '.') mainCls[i] = '/';

            manifest->type = 0;
            memcpy(manifest->mainCls, mainCls, len);
            manifest->mainCls[len] = 0;
        }
        else if (strncmp(line, "MIDlet-1:", 9) == 0) {
            char *name = getNextValue(&line[9], ',', &len);
            char *icon = getNextValue(&name[len], ',', &len);
            char *mainCls = getNextValue(&icon[len], ',', &len);
            if (len == 0 || len >= sizeof(manifest->mainCls)) goto exit;
            for (uint32_t i = 0; i < len; i++) if (mainCls[i] == '.') mainCls[i] = '/';

            manifest->type = 1;
            memcpy(manifest->mainCls, mainCls, len);
            manifest->mainCls[len] = 0;
        }
        else if (strncmp(line, "Launch-Mode:", 12) == 0) {
            const char *mode = getNextValue(line + 12, ',', &len);
            manifest->launchMode = (len == 8 && strncmp(mode, "standard", 8) == 0) ? 1 : 0;
        }
    }
    ret = true;
exit:
    zip.close();
    return ret;
}

static FProcess *runNewApp(Manifest *manifest, JStringArray *args) {
    FProcess *process = FlintOS::newProcess();
    if (process == NULL) return NULL;
    do {
        if (!process->setProgram(manifest->jarFile)) break;

        JClass *mainCls = process->findClass(NULL, manifest->mainCls);
        if (mainCls == NULL) break;

        if (manifest->type == 0) {    /* Normal application */
            MethodInfo *method = mainCls->getClassLoader()->getMainMethodInfo(NULL);
            if (method == NULL) break;

            if (!process->start(method, 1, args)) break;
        }
        else {                      /* J2ME application */
            static constexpr ConstNameAndType startAppName("startApp", "(Ljava/lang/Class;)V");

            JClass *ams = process->findClass(NULL, "flint/midp/AMS");
            if (ams == NULL) break;

            MethodInfo *method = process->findMethod(NULL, ams, (ConstNameAndType *)&startAppName);
            if (method == NULL) break;

            if (!process->start(method, 1, mainCls)) break;
        }
        FlintOS::setForeground(process);
        return process;
    } while (0);
    flintTerminated(process);
    return NULL;
}

static const char *getExtensionName(const char *fileName) {
    int32_t endIdx = strlen(fileName) - 1;
    while (endIdx >= 0 && fileName[endIdx] != '.') endIdx--;
    return (endIdx >= 0) ? &fileName[endIdx] : NULL;
}

FProcess *FlintOS::open(const char *file, void *args) {
    if (file == NULL) return NULL;
    if (FlintAPI::IO::finfo(file, NULL) == FlintAPI::IO::FILE_RESULT_OK) {
        const char *extName = getExtensionName(file);
        if (strcasecmp(extName, ".jar") == 0) {
            Manifest manifest;
            if (!readManifest(file, &manifest)) return NULL;
            if (manifest.launchMode == 1)   /* standard mode - always create new */
                return runNewApp(&manifest, (JStringArray *)args);
            else {                          /* single mode - create only if it does not already exist */
                uint32_t hash = Hash(file);
                FProcess *process = processList.find([&file, &hash](FProcess *item) -> bool {
                    const char *program = item->getProgram();
                    return (hash == ((uint32_t *)program)[-1]) && (strcmp(file, program) == 0);
                });
                if (process != NULL)
                    return process;
                return runNewApp(&manifest, (JStringArray *)args);
            }
        }
    }
    return NULL;
}

FList<FProcess> *FlintOS::getProcesses(void) {
    return &processList;
}

void FlintOS::setHomeApp(FProcess *process) {
    homeApp = process;
}

bool FlintOS::isForeground(FProcess *process, bool checkOnly) {
    if (process == currentForeground)
        return true;
    if (process == allowForeground) {
        if (!checkOnly) {
            if (currentForeground != NULL)
                currentForeground->getEventQueue()->postEvent(&backgroundEvent);
            currentForeground = allowForeground;
        }
        return true;
    }
    return false;
}

void FlintOS::setForeground(FProcess *process) {
    allowForeground = process;
    process->getEventQueue()->postEvent(&foregroundEvent);
}

bool FlintOS::postEvent(const FEvent *event) {
    if (currentForeground == NULL) return false;

    bool ret = false;
    lock();
    if (currentForeground != NULL)
        ret = currentForeground->getEventQueue()->postEvent(event);
    unlock();
    return ret;
}

void FlintOS::lock() {
    fosMutex.lock();
}

void FlintOS::unlock() {
    fosMutex.unlock();
}
