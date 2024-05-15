#pragma once
#include "../Misc/Math.h"
#include "Framebuffer.h"
#include "SimpleFonts.h" 
#include <stdint.h>

/* COLORS */

#define COLOR_WHITE 0x00ffffff
#define COLOR_RED 0x00ff0000
#define COLOR_GREEN 0x0000ff00
#define COLOR_BLUE 0x000000ff
#define COLOR_LIGHT_RED 0x00f94343
#define COLOR_LIGHT_GREEN 0x0064fc64
#define COLOR_LIGHT_BLUE 0x004343f9
#define COLOR_YELLOW 0x00ffff00
#define COLOR_MAGENTA 0x00ff00ff
#define COLOR_CYAN 0x0000ffff
#define COLOR_BROWN 0x00a05908
#define COLOR_LIGHT_GRAY 0x009e9e9e
#define COLOR_GRAY 0x00666666
#define COLOR_DARK_GRAY 0x00444444
#define COLOR_BLACK 0x00000000

class BasicRenderer{
    public:
    BasicRenderer(Framebuffer* NewTargetFramebuffer, PSF1_FONT* NewPSF1Font);
    Point CursorPosition;
    Framebuffer* TargetFramebuffer;
    PSF1_FONT* PSF1Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    unsigned int Colour;
    unsigned int ClearColour;
    void Print(const char* String, unsigned int Color = 0xffffffff);
    void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
    void PutChar(char Char);
    void PutPix(uint32_t X, uint32_t Y, uint32_t Colour);
    uint32_t GetPix(uint32_t X, uint32_t Y);
    void ClearChar();
    void Clear();
    void Next();
    void DrawOverlayMouseCursor(uint8_t* MouseCursor, Point Position, uint32_t Colour);
    void ClearMouseCursor(uint8_t* MouseCursor, Point Position);
    bool MouseDrawn;
    void TickCursor();
    void ScrollUp(int lines);
    void ScrollCharUp(int lines);
    private:
    bool TextCursorShown;
    void ScrollSingleCharUp();
};

extern BasicRenderer* GlobalRenderer;