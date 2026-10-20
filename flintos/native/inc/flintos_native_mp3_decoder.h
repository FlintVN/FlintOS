
#ifndef __FLINTOS_NATIVE_MP3_DECODER_H
#define __FLINTOS_NATIVE_MP3_DECODER_H

#include "flint_native.h"

jint NativeMp3Decoder_Init(FNIEnv *env);
jint NativeMp3Decoder_Decode(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len);
jvoid NativeMp3Decoder_Close(FNIEnv *env, jobject obj);

inline constexpr NativeMethod mp3DecoderMethods[] = {
    NATIVE_METHOD("init",   "()I",     NativeMp3Decoder_Init),
    NATIVE_METHOD("decode", "([BII)I", NativeMp3Decoder_Decode),
    NATIVE_METHOD("close",  "()V",     NativeMp3Decoder_Close),
};

#endif /* __FLINTOS_NATIVE_MP3_DECODER_H */
