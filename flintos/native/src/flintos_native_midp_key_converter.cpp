
#include "flintos_native_midp_key_converter.h"

#define UP              1
#define DOWN            6
#define LEFT            2
#define RIGHT           5
#define FIRE            8
#define GAME_A          9
#define GAME_B          10
#define GAME_C          11
#define GAME_D          12

#define KEY_NUM0        48
#define KEY_NUM1        49
#define KEY_NUM2        50
#define KEY_NUM3        51
#define KEY_NUM4        52
#define KEY_NUM5        53
#define KEY_NUM6        54
#define KEY_NUM7        55
#define KEY_NUM8        56
#define KEY_NUM9        57
#define KEY_STAR        42
#define KEY_POUND       35

jint NativeKeyConverter_GetKeyCode(FNIEnv *env, jint gameAction) {
    (void)env;
    switch(gameAction) {
        case UP: return KEY_NUM2;
        case DOWN: return KEY_NUM8;
        case LEFT: return KEY_NUM4;
        case RIGHT: return KEY_NUM6;
        case FIRE: return KEY_NUM5;
        case GAME_A: return KEY_NUM1;
        case GAME_B: return KEY_NUM3;
        case GAME_C: return KEY_NUM7;
        case GAME_D: return KEY_NUM9;
        default: return 0;
    }
}

jint NativeKeyConverter_GetGameAction(FNIEnv *env, jint keyCode) {
    (void)env;
    switch(keyCode) {
        case KEY_NUM2: return UP;
        case KEY_NUM8: return DOWN;
        case KEY_NUM4: return LEFT;
        case KEY_NUM6: return RIGHT;
        case KEY_NUM5: return FIRE;
        case KEY_NUM1: return GAME_A;
        case KEY_NUM3: return GAME_B;
        case KEY_NUM7: return GAME_C;
        case KEY_NUM9: return GAME_D;
        default: return 0;
    }
}

jint NativeKeyConverter_GetSystemKey(FNIEnv *env, jint keyCode) {
    // TODO
    (void)env;
    (void)keyCode;
    return 0;
}

jstring NativeKeyConverter_GetKeyName(FNIEnv *env, jint keyCode) {
    // TODO
    return env->newString("%d", keyCode);
}
