
#include <stddef.h>
#include <stdlib.h>
#include "flint_array_object.h"
#include "flint.h"
#include "flint_execution.h"
#include "flint_java_string.h"
#include "flint_zip_file_reader.h"
#include "flint_system_api.h"
#include "flintos_default_conf.h"
#include "flintos_display_service.h"
#include "flintos_native_midp.h"

extern "C" size_t tinfl_decompress_mem_to_mem(
    void *output,
    size_t outputLength,
    const void *source,
    size_t sourceLength,
    int flags
);

static constexpr int TINFL_NON_WRAPPING_OUTPUT = 4;

static void ThrowNullPointer(FNIEnv *env, const char *message) {
    env->throwNew(env->findClass("java/lang/NullPointerException"), message);
}

static void ThrowIllegalArgument(FNIEnv *env, const char *message) {
    env->throwNew(env->findClass("java/lang/IllegalArgumentException"), message);
}

jvoid NativeMidpLcd_Init(FNIEnv *env) {
    (void)env;
    /* FlintOS initializes the display before it starts a MIDlet. */
}

jint NativeMidpLcd_Width(FNIEnv *env) {
    (void)env;
    return DISPLAY_WIDTH;
}

jint NativeMidpLcd_Height(FNIEnv *env) {
    (void)env;
    return DISPLAY_HEIGHT;
}

jvoid NativeMidpLcd_Present(FNIEnv *env, jbyteArray frameBuffer) {
    if(frameBuffer == NULL) {
        ThrowNullPointer(env, "frameBuffer");
        return;
    }

    constexpr uint32_t requiredLength = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2;
    if(frameBuffer->getLength() != requiredLength) {
        ThrowIllegalArgument(env, "Invalid frame buffer length");
        return;
    }

    DisplaySrv::write(
        0,
        0,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        (uint8_t *)frameBuffer->getData()
    );
}

jint NativeMidpLcd_ReadKey(FNIEnv *env) {
    (void)env;
    return -1;
}

jvoid NativeMidpTouch_Init(FNIEnv *env) {
    (void)env;
}

jint NativeMidpTouch_Poll(FNIEnv *env) {
    (void)env;
    return -1;
}

jvoid NativeMidpTouch_Boost(FNIEnv *env) {
    (void)env;
    FlintAPI::Thread::yield();
}

jbyteArray NativeMidpResourceLoader_ReadProgramResource(FNIEnv *env, jstring name) {
    if(name == NULL) {
        ThrowNullPointer(env, "name");
        return NULL;
    }

    const char *programPath = ((FExec *)env)->getFlint()->getProgram();
    if(programPath == NULL)
        return NULL;

    ZipFileReader zip((FExec *)env, programPath);
    if(!zip.open())
        return NULL;
    if(!zip.gotoFile(name->getAscii(), name->getLength())) {
        zip.close();
        return NULL;
    }

    uint16_t method = zip.getCompressionMethod();
    uint32_t packedSize = zip.getCompressedSize();
    uint32_t unpackedSize = zip.getUncompressedSize();
    jbyteArray output = env->newByteArray(unpackedSize);
    if(output == NULL) {
        zip.close();
        return NULL;
    }

    bool success = false;
    if(method == 0 && packedSize == unpackedSize) {
        success = zip.read(output->getData(), unpackedSize) == (int32_t)unpackedSize;
    }
    else if(method == 8) {
        uint8_t *packed = (uint8_t *)malloc(packedSize);
        if(packed != NULL) {
            if(zip.read(packed, packedSize) == (int32_t)packedSize) {
                size_t decoded = tinfl_decompress_mem_to_mem(
                    output->getData(), unpackedSize, packed, packedSize,
                    TINFL_NON_WRAPPING_OUTPUT
                );
                success = decoded == unpackedSize;
            }
            free(packed);
        }
    }
    zip.close();
    return success ? output : NULL;
}
