
#include <new>
#include <string.h>
#include "flint.h"
#include "flint_java_string.h"
#include "flintos_hal_devices.h"
#include "flintos_native_wifi.h"

static FMutex wifiLock;

static bool checkIsSupported(FNIEnv *env) {
    if(HAL::Devices::wifi() == NULL) {
        env->throwNew(env->findClass("java/lang/UnsupportedOperationException"), "Wi-Fi is not supported");
        return false;
    }
    return true;
}

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
    (void)env;
    return HAL::Devices::wifi() != NULL;
}

jvoid NativeWiFi_Connect(FNIEnv *env, jstring ssid, jstring password, jint authMode) {
    if(!checkIsSupported(env)) return;
    if(!checkParams(env, ssid, password, authMode)) return;

    uint32_t ssidLen = ssid->getLength();
    uint32_t passwordLen = password ? password->getLength() : 0;
    const char *ssidText = ssid->getAscii();
    const char *passwordText = password ? password->getAscii() : NULL;

    wifiLock.lock();
    bool ret = HAL::Devices::wifi()->connect(ssidText, ssidLen, passwordText, passwordLen, authMode);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while connecting to wifi");
}

jbool NativeWiFi_IsConnected(FNIEnv *env) {
    (void)env;
    if(!checkIsSupported(env)) return false;
    wifiLock.lock();
    bool ret = HAL::Devices::wifi()->isConnected();
    wifiLock.unlock();
    return ret;
}

static jobject createAccessPointRecordObj(FNIEnv *env, HAL::WiFi::ApRecordType *apRecord) {
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
    if(!checkIsSupported(env)) return NULL;
    HAL::WiFi::ApRecordType apInfo;
    wifiLock.lock();
    if(checkReturn(env, HAL::Devices::wifi()->getAPinfo(&apInfo), "getAPinfo error")) {
        wifiLock.unlock();
        jobject obj = createAccessPointRecordObj(env, &apInfo);
        return (obj != NULL) ? obj : NULL;
    }
    wifiLock.unlock();
    return NULL;
}

jvoid NativeWiFi_Disconnect(FNIEnv *env) {
    if(!checkIsSupported(env)) return;
    wifiLock.lock();
    HAL::Devices::wifi()->disconnect();
    wifiLock.unlock();
}

jvoid NativeWiFi_SoftAP(FNIEnv *env, jstring ssid, jstring password, jint authMode, jint channel, jint maxConnection) {
    if(!checkIsSupported(env)) return;
    if(!checkParams(env, ssid, password, authMode)) return;

    uint32_t ssidLen = ssid->getLength();
    uint32_t passwordLen = password ? password->getLength() : 0;
    const char *ssidText = ssid->getAscii();
    const char *passwordText = password ? password->getAscii() : 0;

    wifiLock.lock();
    bool ret = HAL::Devices::wifi()->softAP(ssidText, ssidLen, passwordText, passwordLen, authMode, channel, maxConnection);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while connecting to wifi");
}

jvoid NativeWiFi_SoftAPdisconnect(FNIEnv *env) {
    if(!checkIsSupported(env)) return;
    wifiLock.lock();
    HAL::Devices::wifi()->softAPDisconnect();
    wifiLock.unlock();
}

jvoid NativeWiFi_StartScan(FNIEnv *env, jbool blocked) {
    if(!checkIsSupported(env)) return;
    wifiLock.lock();
    bool ret = HAL::Devices::wifi()->startScan(blocked);
    wifiLock.unlock();
    checkReturn(env, ret, "An error occurred while starting scan");
}

jobjectArray NativeWiFi_GetScanResult(FNIEnv *env) {
    if(!checkIsSupported(env)) return NULL;
    wifiLock.lock();
    int32_t count = HAL::Devices::wifi()->getScanAPCount();
    if(!checkReturn(env, count >= 0, "An error occurred while getting AP number")) {
        wifiLock.unlock();
        return NULL;
    }

    if(count == 0) {
        HAL::Devices::wifi()->scanClear();
        wifiLock.unlock();
        return NULL;
    }

    jobjectArray arrayObj = env->newObjectArray(env->findClass("flint/net/AccessPointRecord"), count);
    if(arrayObj == NULL) {
        HAL::Devices::wifi()->scanClear();
        wifiLock.unlock();
        return NULL;
    }
    arrayObj->clearData();
    JObject **data = arrayObj->getData();
    for(uint16_t i = 0; i < count; i++) {
        HAL::WiFi::ApRecordType apRecords;
        bool ret = HAL::Devices::wifi()->getScanAPInfo(&apRecords);
        if(!checkReturn(env, ret, "An error occurred while getting AP record")) {
            HAL::Devices::wifi()->scanClear();
            wifiLock.unlock();
            return NULL;
        }
        jobject aprObj = createAccessPointRecordObj(env, &apRecords);
        if(aprObj == NULL) {
            HAL::Devices::wifi()->scanClear();
            wifiLock.unlock();
            while(i) env->freeObject(data[--i]);
            env->freeObject(arrayObj);
            return NULL;
        }
        data[i] = aprObj;
    }

    HAL::Devices::wifi()->scanClear();
    wifiLock.unlock();
    return arrayObj;
}

jvoid NativeWiFi_StopScan(FNIEnv *env) {
    if(!checkIsSupported(env)) return;
    wifiLock.lock();
    HAL::Devices::wifi()->stopScan();
    wifiLock.unlock();
}
