
#ifndef __FLINTOS_NATIVE_MIDP_IMAGE_H
#define __FLINTOS_NATIVE_MIDP_IMAGE_H

#include "flint_native.h"

jbyteArray NativeMidpImage_PngDecode(FNIEnv *env, jbyteArray data, jint off, jint len, jintArray wh);

inline constexpr NativeMethod midpImageMethods[] = {
    NATIVE_METHOD("pngDecode", "([BII[I)[B", NativeMidpImage_PngDecode),
};

#endif /* __FLINTOS_NATIVE_MIDP_IMAGE_H */
