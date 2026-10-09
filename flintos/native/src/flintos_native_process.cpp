
#include "flintos.h"
#include "flint_java_string.h"
#include "flint_array_object.h"
#include "flintos_native_process.h"

class JProcess : public JObject {
public:
    jint getHandle() { return getFieldByIndex(0)->getInt32(); }
    jstring getName() { return (jstring)getFieldByIndex(1)->getObj(); }
    jarray getArgs() { return (jarray)getFieldByIndex(2)->getObj(); }

    void setHandle(jint val) { getFieldByIndex(0)->setInt32(val); }
    void setName(jstring val) { getFieldByIndex(1)->setObj(val); }
};

static bool ResolvePath(FNIEnv *env, jstring name, char *buff, uint32_t buffSize) {
    if (name == NULL) {
        env->throwNew(env->findClass("java/lang/NullPointerException"), "name cannot be null");
        return false;
    }
    if (((FExec *)env)->getFlint()->resolvePath(name->getAscii(), name->getLength(), buff, buffSize) == -1) {
        jclass excpCls = env->findClass("java/lang/IllegalArgumentException");
        env->throwNew(excpCls, "Unable to resolve the path, file name too long leads to insufficient buffer size");
        return false;
    }
    return true;
}

jvoid NativeProcess_Start(FNIEnv *env, jobject obj) {
    char buff[FILE_NAME_BUFF_SIZE];
    JProcess *p = (JProcess *)obj;
    uint32_t handle = p->getHandle();
    FProcess *fprocess;
    if (handle != -1) {
        fprocess = FlintOS::getProcesses()->find([&handle](FProcess *item) -> bool { return (jint)item == handle; });
        if (fprocess != NULL) {
            env->throwNew(env->findClass("java/lang/IllegalStateException"), "Cannot restart an existing process");
            return;
        }
    }
    if (!ResolvePath(env, p->getName(), buff, sizeof(buff))) return;
    fprocess = FlintOS::open(buff, p->getArgs());
    if (fprocess == NULL) {
        jclass excpCls = env->findClass("java/lang/IllegalArgumentException");
        env->throwNew(excpCls, "Process start failed");
        return;
    }
    p->setHandle((jint)fprocess);
}

jvoid NativeProcess_Close(FNIEnv *env, jobject obj) {
    JProcess *p = (JProcess *)obj;
    jint handle = p->getHandle();
    if (handle != -1) {
        FlintOS::lock();
        FProcess *fprocess = FlintOS::getProcesses()->find([&handle](FProcess *item) -> bool { return (jint)item == handle; });
        if (fprocess != NULL)
            fprocess->terminateRequest();
        FlintOS::unlock();
        p->setHandle(-1);
    }
}

jobjectArray NativeProcess_GetProcesses(FNIEnv *env) {
    FlintOS::lock();
    FList<FProcess> *processes = FlintOS::getProcesses();
    uint32_t len = (processes != NULL) ? processes->length() : 0;
    jobjectArray arrayObj = env->newObjectArray(env->findClass("flint/system/Process"), len);
    if (arrayObj == NULL) return NULL;
    JObject **data = arrayObj->getData();
    if (len > 0) {
        FProcess *root = processes->find([](FProcess *) -> bool { return true; });
        FProcess *node = root;
        for (uint32_t i = 0; i < len; i++) {
            data[i] = env->newObject(env->findClass("flint/system/Process"));
            jstring name = env->newString(node->getProgram());
            if (name == NULL || data[i] == NULL) {
                FlintOS::unlock();
                if (data[i] != NULL) i++;
                while (i) env->freeObject(data[--i]);
                if (name != NULL) env->freeObject(name);
                env->freeObject(arrayObj);
                return NULL;
            }
            ((JProcess *)data[i])->setHandle((jint)node);
            ((JProcess *)data[i])->setName(name);
            node = (FProcess *)node->next;
        }
    }
    FlintOS::unlock();
    return arrayObj;
}
