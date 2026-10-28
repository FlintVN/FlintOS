
#include "flintos.h"
#include "flint_system_api.h"
#include "flintos_debugger.h"
#include "flintos_hal_devices.h"
#include "flintos_default_conf.h"
#include "flintos_default_apps.h"
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

extern char *getNextValue(char *value, char seperator, uint32_t *len);

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

static void initDefaultApps() {
    char buff[FILE_NAME_BUFF_SIZE];

    FileReader reader(NULL, "/sys/default.ini");
    if (!reader.open()) return;

    while (reader.readLine(buff, sizeof(buff)) != -1) {
        char *line = trim(buff);
        uint32_t len = strcspn(line, " :");
        char *program = (char *)FlintAPI::System::malloc(len + 1);
        memcpy(program, line, len);
        program[len] = 0;

        line = &line[len + 1];
        len = 0;
        while (true) {
            line = getNextValue(&line[len], ',', &len);
            if (len == 0) break;
            DefaultApps::add(line, len, program);
        }
    }
    reader.close();
}

static void setHomeApp(FProcess *process) {
    homeApp = process;
}

static void startup() {
    bool isFirst = true;
    char path[FILE_NAME_BUFF_SIZE];

    FileReader reader(NULL, "/sys/startup.ini");
    if (!reader.open()) return;

    while (reader.readLine(path, sizeof(path)) != -1) {
        char *text = trim(path);
        FProcess *process = FlintOS::open(NULL, text);
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
        FlintAPI::Thread::create((void (*)(void *))DisplaySrv::mainTask, NULL, 2048, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    }
    if (HAL::Devices::keyBoard())
        HAL::Devices::keyBoard()->init();
    if (HAL::Devices::touch())
        HAL::Devices::touch()->init();
    FlintAPI::Thread::create((void (*)(void *))InputSrv::mainTask, NULL, 2048, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    if (HAL::Devices::audio() != NULL) {
        HAL::Devices::audio()->init();
        AudioSrv::setVolume(100);
        FlintAPI::Thread::create((void (*)(void *))AudioSrv::mainTask, NULL, 2048, FlintAPI::Thread::THREAD_PRIORITY_HIGH);
    }
    if (HAL::Devices::wifi() != NULL)
        HAL::Devices::wifi()->init();
    ZipClassCache::init();
    initDefaultApps();
    startup();
    FlintAPI::Thread::create((void (*)(void *))debuggerTask, NULL, 6144, FlintAPI::Thread::THREAD_PRIORITY_MEDIUM);
}

FProcess *FlintOS::newProcess(void) {
    FProcess *process = (FProcess *)FlintAPI::System::malloc(sizeof(FProcess));
    if (process == NULL) return NULL;
    new (process)FProcess();
    process->terminatedCallback(flintTerminated);
    fosMutex.lock();
    processList.add(process);
    fosMutex.unlock();
    return process;
}

typedef struct {
    uint8_t type : 1;
    uint8_t launchMode : 1;
    const char *jarFile;
    char mainCls[FILE_NAME_BUFF_SIZE];
} Manifest;

static bool readManifest(FExec *ctx, const char *jarPath, Manifest *manifest) {
    uint32_t len;
    bool ret = false;
    char buff[FILE_NAME_BUFF_SIZE];
    ZipFileReader zip(ctx, jarPath);
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

static JStringArray *cloneArgs(FExec *ctx, FProcess *process, JStringArray *args) {
    JClass *strCls = process->findClass(ctx, "java/lang/String");
    if (strCls == NULL) return NULL;

    JClass *byteArrCls = process->findClass(ctx, "[B");
    if (byteArrCls == NULL) return NULL;

    uint32_t argc = args->getLength();
    JStringArray *args2 = (JStringArray *)process->newArray(ctx, process->findClass(ctx, "[Ljava/lang/String;"), argc);
    if (args2 == NULL) return NULL;

    for (uint32_t i = 0; i < argc; i++) {
        JString *str1 = args->getData()[i];
        JString *str2 = NULL;
        if (str1 != NULL) {
            str2 = (JString *)process->newObject(ctx, strCls);
            if (str2 == NULL) return NULL;

            JByteArray *bytes = (JByteArray *)process->newArray(ctx, byteArrCls, str1->getValue()->getLength());
            if (bytes == NULL) return NULL;

            memcpy(bytes->getData(), str1->getValue()->getData(), bytes->getLength());
            str2->setHash(str1->getHash());
            str2->setHashIsZero(str1->getHashIsZero());
            str2->setValue(bytes);
        }
        args2->getData()[i] = str2;
    }

    return args2;
}

static JStringArray *createArgs(FExec *ctx, FProcess *process, uint32_t argc, const char *argv[]) {
    JStringArray *args = (JStringArray *)process->newArray(ctx, process->findClass(ctx, "[Ljava/lang/String;"), argc);
    if (args == NULL) return NULL;

    for (uint32_t i = 0; i < argc; i++) {
        JString *str = process->newString(ctx, argv[i]);
        if (str == NULL) return NULL;
        args->getData()[i] = str;
    }

    return args;
}

static bool runNewApp(FExec *ctx, FProcess *process, Manifest *manifest, JStringArray *args) {
    if (!process->setProgram(manifest->jarFile)) return false;

    JClass *mainCls = process->findClass(ctx, manifest->mainCls);
    if (mainCls == NULL) return false;

    if (manifest->type == 0) {  /* Normal application */
        MethodInfo *method = mainCls->getClassLoader()->getMainMethodInfo(ctx);
        return process->start(method, 1, args);
    }
    else {                      /* J2ME application */
        static constexpr ConstNameAndType startAppName("startApp", "(Ljava/lang/Class;)V");
        JClass *ams = process->findClass(ctx, "flint/midp/AMS");
        MethodInfo *method = process->findMethod(ctx, ams, (ConstNameAndType *)&startAppName);
        return process->start(method, 1, mainCls);
    }
}

static const char *getExtensionName(const char *fileName) {
    int32_t endIdx = strlen(fileName) - 1;
    while (endIdx >= 0 && fileName[endIdx] != '.') endIdx--;
    return (endIdx >= 0) ? &fileName[endIdx + 1] : NULL;
}

static FProcess *findProcess(const char *file) {
    uint32_t hash = Hash(file);
    FProcess *process = processList.find([&file, &hash](FProcess *item) -> bool {
        const char *program = item->getProgram();
        return (hash == ((uint32_t *)program)[-1]) && (strcmp(file, program) == 0);
    });
    if (process != NULL) {
        FlintOS::setForeground(process);
        return process;
    }
    return NULL;
}

static FProcess *openApp(FExec *ctx, const char *file, JStringArray *args) {
    Manifest manifest;
    if (!readManifest(ctx, file, &manifest)) return NULL;

    if (manifest.launchMode == 0) {             /* single mode - create only if it does not already exist or the argument needs to be changed */
        fosMutex.lock();
        FProcess *process = findProcess(file);
        if (process != NULL) {
            if (args == NULL) {
                fosMutex.unlock();
                return process;
            }
            else
                process->terminateRequest();
        }
        fosMutex.unlock();
    }

    FProcess *process = FlintOS::newProcess();
    if (process == NULL) return NULL;

    if (manifest.type == 0 && args != NULL) {   /* Normal application */
        args = cloneArgs(ctx, process, args);
        if (args == NULL) {
            flintTerminated(process);
            return NULL;
        }
    }

    if (!runNewApp(ctx, process, &manifest, args)) {
        flintTerminated(process);
        return NULL;
    }
    FlintOS::setForeground(process);
    return process;
}

static FProcess *openApp(FExec *ctx, const char *file, uint32_t argc, const char *argv[]) {
    Manifest manifest;
    if (!readManifest(ctx, file, &manifest)) return NULL;

    if (manifest.launchMode == 0) {                         /* single mode - create only if it does not already exist or the argument needs to be changed */
        fosMutex.lock();
        FProcess *process = findProcess(file);
        if (process != NULL) {
            if (argc == 0 || argv == NULL) {
                fosMutex.unlock();
                return process;
            }
            else
                process->terminateRequest();
        }
        fosMutex.unlock();
    }

    FProcess *process = FlintOS::newProcess();
    if (process == NULL) return NULL;

    JStringArray *args = NULL;
    if (manifest.type == 0 && argc > 0 && argv != NULL) {   /* Normal application */
        args = createArgs(ctx, process, argc, argv);
        if (args == NULL) {
            flintTerminated(process);
            return NULL;
        }
    }

    if (!runNewApp(ctx, process, &manifest, args)) {
        flintTerminated(process);
        return NULL;
    }
    FlintOS::setForeground(process);
    return process;
}

FProcess *FlintOS::open(FExec *ctx, const char *file, JStringArray *args) {
    if (file == NULL) return NULL;
    if (FlintAPI::IO::finfo(file, NULL) == FlintAPI::IO::FILE_RESULT_OK) {
        const char *extName = getExtensionName(file);
        if (strcasecmp(extName, "jar") == 0)
            return openApp(ctx, file, args);
        else if (extName != NULL) {
            const char *app = DefaultApps::find(extName);
            if (app != NULL && FlintAPI::IO::finfo(app, NULL) == FlintAPI::IO::FILE_RESULT_OK) {
                const char *argv[] = {file};
                return openApp(ctx, app, 1, argv);
            }
        }
    }
    return NULL;
}

FList<FProcess> *FlintOS::getProcesses(void) {
    return &processList;
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
    if (event->type == KEY_EVENT && event->data[0] == HAL::KEY_RELEASED && event->data[1] == HAL::KEYCODE_HOME) {
        if (homeApp != NULL && homeApp != currentForeground && homeApp != allowForeground)
            setForeground(homeApp);
        return true;
    }
    if (currentForeground == NULL)
        return false;
    bool ret = false;
    fosMutex.lock();
    if (currentForeground != NULL)
        ret = currentForeground->getEventQueue()->postEvent(event);
    fosMutex.unlock();
    return ret;
}

void FlintOS::lock() {
    fosMutex.lock();
}

void FlintOS::unlock() {
    fosMutex.unlock();
}
