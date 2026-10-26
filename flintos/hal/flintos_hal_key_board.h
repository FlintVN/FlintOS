
#ifndef __FLINTOS_HAL_KEY_BOARD_H
#define __FLINTOS_HAL_KEY_BOARD_H

#include <stdint.h>

namespace HAL {
    enum KeyState : uint8_t {
        KEY_NONE = 0,
        KEY_PRESSED = 1,
        KEY_RELEASED = 2,
    };

    enum KeyCode : uint32_t {
        KEYCODE_HOME = 3,
        KEYCODE_BACK = 4,
        KEYCODE_VOLUME_UP = 24,
        KEYCODE_VOLUME_DOWN = 25,
        KEYCODE_POWER = 26,
        
        KEYCODE_DPAD_UP = 19,
        KEYCODE_DPAD_DOWN = 20,
        KEYCODE_DPAD_LEFT = 21,
        KEYCODE_DPAD_RIGHT = 22,
        KEYCODE_DPAD_CENTER = 23,
        KEYCODE_PAGE_UP = 92,
        KEYCODE_PAGE_DOWN = 93,
        KEYCODE_MOVE_HOME = 122,
        KEYCODE_MOVE_END = 123,

        KEYCODE_KEYCODE_0 = 7,
        KEYCODE_KEYCODE_1 = 8,
        KEYCODE_KEYCODE_2 = 9,
        KEYCODE_KEYCODE_3 = 10,
        KEYCODE_KEYCODE_4 = 11,
        KEYCODE_KEYCODE_5 = 12,
        KEYCODE_KEYCODE_6 = 13,
        KEYCODE_KEYCODE_7 = 14,
        KEYCODE_KEYCODE_8 = 15,
        KEYCODE_KEYCODE_9 = 16,

        KEYCODE_KEYCODE_A = 29,
        KEYCODE_KEYCODE_B = 30,
        KEYCODE_KEYCODE_C = 31,
        KEYCODE_KEYCODE_D = 32,
        KEYCODE_KEYCODE_E = 33,
        KEYCODE_KEYCODE_F = 34,
        KEYCODE_KEYCODE_G = 35,
        KEYCODE_KEYCODE_H = 36,
        KEYCODE_KEYCODE_I = 37,
        KEYCODE_KEYCODE_J = 38,
        KEYCODE_KEYCODE_K = 39,
        KEYCODE_KEYCODE_L = 40,
        KEYCODE_KEYCODE_M = 41,
        KEYCODE_KEYCODE_N = 42,
        KEYCODE_KEYCODE_O = 43,
        KEYCODE_KEYCODE_P = 44,
        KEYCODE_KEYCODE_Q = 45,
        KEYCODE_KEYCODE_R = 46,
        KEYCODE_KEYCODE_S = 47,
        KEYCODE_KEYCODE_T = 48,
        KEYCODE_KEYCODE_U = 49,
        KEYCODE_KEYCODE_V = 50,
        KEYCODE_KEYCODE_W = 51,
        KEYCODE_KEYCODE_X = 52,
        KEYCODE_KEYCODE_Y = 53,
        KEYCODE_KEYCODE_Z = 54,

        KEYCODE_STAR = 17,
        KEYCODE_POUND = 18,

        KEYCODE_TAB = 61,
        KEYCODE_SPACE = 62,
        KEYCODE_ENTER = 66,
        KEYCODE_DEL = 67,
        KEYCODE_FORWARD_DEL = 112,
        KEYCODE_GRAVE = 68,
        KEYCODE_MINUS = 69,
        KEYCODE_EQUALS = 70,
        KEYCODE_PLUS = 81,

        KEYCODE_LEFT_BRACKET = 71,
        KEYCODE_RIGHT_BRACKET = 72,
        KEYCODE_BACKSLASH = 73,
        KEYCODE_SEMICOLON = 74,
        KEYCODE_APOSTROPHE = 75,
        KEYCODE_SLASH = 76,
    };

    class KeyBoard {
    public:
        virtual void init(void) const = 0;
        virtual KeyState read(uint32_t *keyCode) const = 0;
    };
}

#endif /* __FLINTOS_HAL_KEY_BOARD_H */
