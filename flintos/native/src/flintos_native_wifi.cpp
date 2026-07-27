
#include <new>
#include <string.h>
#include "flint.h"
#include "flintos_devices.h"
#include "flint_java_string.h"
#include "flintos_native_wifi.h"

static FMutex wifiLock;

static bool checkParams(FNIEnv *env, jstring ssid, jstring password, uint32_t authMode) {
    if((ssid == NULL) || ((password == NULL) && (authMode != 0))) {
        if(ssid == NULL) {
            env->throwNew(env->findClass("java/lang/NullPointerException"), "ssid cannot be null object");
            return false;
        }
        else {
            env->throwNew(env->findClass("java/lang/NullPointerException"), "password cannot be null object");
            return false;
        }
    }

    uint32_t ssidLen = ssid->getLength();
    uint32_t passwordLen = password ? password->getLength() : 0;
    if((ssidLen > 32) || (passwordLen > 64)) {
        if(ssidLen > 32) {
            env->throwNew(env->findClass("java/lang/IllegalArgumentException"), "ssid value is invalid");
            return false;
        }
        else {
            env->throwNew(env->findClass("java/lang/IllegalArgumentException"), "password value is invalid");
            return false;
        }
    }

    return true;
}

static bool checkReturn(FNIEnv *env, bool ret, const char *msg) {
    if(ret != true) {
        env->throwNew(env->findClass("java/io/UncheckedIOException"), msg);
        return false;
    }
    return true;
}

jbool NativeWiFi_IsSupported(FNIEnv *env) {
    return FDev::WiFi::isSupported();
}

jvoid NativeWiFi_Connect(FNIEnv *env, jstring ssid, jstring password, jint authMode) {
    if(!checkParams(env, ssid, password, authMode)) return;

    uint32_t ssidLen = ssid->getLength();
    uint32_t passwordLen = password ? password->getLength() : 0;
    const char *ssidText = ssid->getAscii();
    const char *passwordText = password ? password->getAscii() : NULL;

    wifiLock.lock();
    bool ret = FDev::WiFi::connect(ssidText, ssidLen, passwordText, passwordLen, authMode);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while connecting to wifi");
}

jbool NativeWiFi_IsConnected(FNIEnv *env) {
    (void)env;
    wifiLock.lock();
    bool ret = FDev::WiFi::isConnected();
    wifiLock.unlock();
    return ret;
}

static jobject createAccessPointRecordObj(FNIEnv *env, FDev::WiFi::ApRecordType *apRecord) {
    jobject aprObj = env->newObject(env->findClass("flint/net/AccessPointRecord"));
    if(aprObj == NULL) return NULL;

    /* mac array */
    jbyteArray macArray = env->newByteArray(6);
    if(macArray == NULL) return NULL;
    memcpy(macArray->getData(), apRecord->mac, 6);

    env->setObjField(env->getFieldId(aprObj, "mac"), macArray);
    jstring ssid = env->newString((char *)apRecord->ssid);
    if(ssid == NULL) {
        env->freeObject(macArray);
        return NULL;
    }
    env->setObjField(env->getFieldId(aprObj, "ssid"), ssid);
    env->setByteField(env->getFieldId(aprObj, "rssi"), apRecord->rssi);
    env->setByteField(env->getFieldId(aprObj, "authMode"), apRecord->authmode);

    return aprObj;
}

jobject NativeWiFi_GetAPinfo(FNIEnv *env) {
    FDev::WiFi::ApRecordType apInfo;
    wifiLock.lock();
    if(checkReturn(env, FDev::WiFi::getAPinfo(&apInfo), "getAPinfo error")) {
        wifiLock.unlock();
        jobject obj = createAccessPointRecordObj(env, &apInfo);
        return (obj != NULL) ? obj : NULL;
    }
    wifiLock.unlock();
    return NULL;
}

jvoid NativeWiFi_Disconnect(FNIEnv *env) {
    wifiLock.lock();
    FDev::WiFi::disconnect();
    wifiLock.unlock();
}

jvoid NativeWiFi_SoftAP(FNIEnv *env, jstring ssid, jstring password, jint authMode, jint channel, jint maxConnection) {
    if(!checkParams(env, ssid, password, authMode)) return;

    uint32_t ssidLen = ssid->getLength();
    uint32_t passwordLen = password ? password->getLength() : 0;
    const char *ssidText = ssid->getAscii();
    const char *passwordText = password ? password->getAscii() : 0;

    wifiLock.lock();
    bool ret = FDev::WiFi::softAP(ssidText, ssidLen, passwordText, passwordLen, authMode, channel, maxConnection);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while connecting to wifi");
}

jvoid NativeWiFi_SoftAPdisconnect(FNIEnv *env) {
    wifiLock.lock();
    FDev::WiFi::softAPDisconnect();
    wifiLock.unlock();
}

jvoid NativeWiFi_StartScan(FNIEnv *env, jbool blocked) {
    wifiLock.lock();
    bool ret = FDev::WiFi::startScan(blocked);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while starting scan");
}

jobjectArray NativeWiFi_GetScanResult(FNIEnv *env) {
    wifiLock.lock();
    int32_t count = FDev::WiFi::getScanAPCount();
    if(!checkReturn(env, count >= 0, "An error occurred while getting AP number")) {
        wifiLock.unlock();
        return NULL;
    }

    if(count == 0) {
        FDev::WiFi::scanClear();
        wifiLock.unlock();
        return NULL;
    }

    jobjectArray arrayObj = env->newObjectArray(env->findClass("flint/net/AccessPointRecord"), count);
    if(arrayObj == NULL) {
        FDev::WiFi::scanClear();
        wifiLock.unlock();
        return NULL;
    }
    arrayObj->clearData();
    JObject **data = arrayObj->getData();
    for(uint16_t i = 0; i < count; i++) {
        FDev::WiFi::ApRecordType apRecords;
        bool ret = FDev::WiFi::getScanAPInfo(&apRecords);
        if(!checkReturn(env, ret, "An error occurred while getting AP record")) {
            FDev::WiFi::scanClear();
            wifiLock.unlock();
            return NULL;
        }
        jobject aprObj = createAccessPointRecordObj(env, &apRecords);
        if(aprObj == NULL) {
            FDev::WiFi::scanClear();
            wifiLock.unlock();
            while(i) env->freeObject(data[--i]);
            env->freeObject(arrayObj);
            return NULL;
        }
        data[i] = aprObj;
    }

    FDev::WiFi::scanClear();
    wifiLock.unlock();
    return arrayObj;
}

jvoid NativeWiFi_StopScan(FNIEnv *env) {
    wifiLock.lock();
    FDev::WiFi::stopScan();
    wifiLock.unlock();
}
