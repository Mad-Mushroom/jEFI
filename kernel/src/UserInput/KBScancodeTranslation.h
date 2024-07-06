#pragma once
#include <stdint.h>

namespace QWERTYKeyboard{

    #define KEY_LEFT_SHIFT 0x2A
    #define KEY_RIGHT_SHIFT 0x36
    #define KEY_ENTER 0x1C
    #define KEY_BACKSPACE 0x0E
    #define KEY_SPACEBAR 0x39

    #define KEY_F1 0x3B
    #define KEY_F2 0x3C
    #define KEY_F3 0x3D
    #define KEY_F4 0x3E
    #define KEY_F5 0x3F
    #define KEY_F6 0x40
    #define KEY_F7 0x41
    #define KEY_F8 0x42
    #define KEY_F9 0x43
    #define KEY_F10 0x44
    #define KEY_F11 0x57
    #define KEY_F12 0x58

    extern const char ASCIITable[];
    char Translate(uint8_t Scancode, bool Uppercase);
}