#pragma once
#include <stdint.h>
#include "../Graphics/BasicRenderer.h"
#include "../UserInput/KBScancodeTranslation.h"
#include "../Misc/CStr.h"
#include "../System/Memory/Memory.h"
#include "../System/KernelUtil.h"
#include "../System/Scheduling/PIT/PIT.h"
#include "../System/PCI/PCI.h"
#include "../System/AHCI/AHCI.h"

#define SHELL_VERSION "0.1.2"

class jShell {
public:
    jShell();
    void HandleKeyPress(uint8_t Scancode);
    void RunShell();
    void Start();
    void ParseCommand();
    void Print(const char* Text, uint32_t Color = 0xffffffff);
    void PutChar(char chr, uint32_t Color = 0xffffffff);
    void Clear(uint32_t Color = 0x00000000);
private:
    void DrawTitlebar();
    void DisplayInfo();
    bool IsLeftShiftPressed;
    bool IsRightShiftPressed;
    char CommandBuffer[16384];
    int CommandBufferSize;
    Point CursorPosition;
    char cmd_argv[64][256];
    int64_t index = 0;
    int64_t index2 = 0;
    int64_t args = 0;
};

extern jShell* MainShell;