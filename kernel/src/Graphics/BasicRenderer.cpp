#include "BasicRenderer.h"

BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* NewTargetFramebuffer, PSF1_FONT* NewPSF1Font)
{
    TargetFramebuffer = NewTargetFramebuffer;
    PSF1Font = NewPSF1Font;
    Colour = 0xffffffff;
    CursorPosition = {0, 0};
}

void BasicRenderer::PutPix(uint32_t X, uint32_t Y, uint32_t Colour){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4)) = Colour;
}

uint32_t BasicRenderer::GetPix(uint32_t X, uint32_t Y){
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::ClearMouseCursor(uint8_t* MouseCursor, Point Position){
    if (!MouseDrawn) return;

    int XMax = 16;
    int YMax = 16;
    int DifferenceX = TargetFramebuffer->Width - Position.X;
    int DifferenceY = TargetFramebuffer->Height - Position.Y;

    if (DifferenceX < 16) XMax = DifferenceX;
    if (DifferenceY < 16) YMax = DifferenceY;

    for (int Y = 0; Y < YMax; Y++){
        for (int X = 0; X < XMax; X++){
            int Bit = Y * 16 + X;
            int Byte = Bit / 8;
            if ((MouseCursor[Byte] & (0b10000000 >> (X % 8))))
            {
                if (GetPix(Position.X + X, Position.Y + Y) == MouseCursorBufferAfter[X + Y *16]){
                    PutPix(Position.X + X, Position.Y + Y, MouseCursorBuffer[X + Y * 16]);
                }
            }
        }
    }
}

void BasicRenderer::DrawOverlayMouseCursor(uint8_t* MouseCursor, Point Position, uint32_t Colour){


    int XMax = 16;
    int YMax = 16;
    int DifferenceX = TargetFramebuffer->Width - Position.X;
    int DifferenceY = TargetFramebuffer->Height - Position.Y;

    if (DifferenceX < 16) XMax = DifferenceX;
    if (DifferenceY < 16) YMax = DifferenceY;

    for (int Y = 0; Y < YMax; Y++){
        for (int X = 0; X < XMax; X++){
            int Bit = Y * 16 + X;
            int Byte = Bit / 8;
            if ((MouseCursor[Byte] & (0b10000000 >> (X % 8))))
            {
                MouseCursorBuffer[X + Y * 16] = GetPix(Position.X + X, Position.Y + Y);
                PutPix(Position.X + X, Position.Y + Y, Colour);
                MouseCursorBufferAfter[X + Y * 16] = GetPix(Position.X + X, Position.Y + Y);

            }
        }
    }

    MouseDrawn = true;
}

void BasicRenderer::Clear(){
    uint64_t FBBase = (uint64_t)TargetFramebuffer->BaseAddress;
    uint64_t BytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t FBHeight = TargetFramebuffer->Height;
    uint64_t FBSize = TargetFramebuffer->BufferSize;

    for (int VerticalScanline = 0; VerticalScanline < FBHeight; VerticalScanline ++){
        uint64_t PixelPtrBase = FBBase + (BytesPerScanline * VerticalScanline);
        for (uint32_t* PixelPtr = (uint32_t*)PixelPtrBase; PixelPtr < (uint32_t*)(PixelPtrBase + BytesPerScanline); PixelPtr ++){
            *PixelPtr = ClearColour;
        }
    }

    CursorPosition = {0, 0};
}

void BasicRenderer::ClearChar(){
    if (CursorPosition.X == 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

    unsigned int XOffset = CursorPosition.X;
    unsigned int YOffset = CursorPosition.Y;

    unsigned int* PixelPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    for (unsigned long Y = YOffset; Y < YOffset + 16; Y++){
        for (unsigned long X = XOffset - 8; X < XOffset; X++){
                    *(unsigned int*)(PixelPtr + X + (Y * TargetFramebuffer->PixelsPerScanLine)) = ClearColour;
        }
    }

    CursorPosition.X -= 8;

    if (CursorPosition.X < 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

}

void BasicRenderer::Next(){
    CursorPosition.X = 0;
    CursorPosition.Y += 16;
}

void BasicRenderer::Print(const char* String, unsigned int Color){
    char* Char = (char*)String;
    unsigned int OldColor = GlobalRenderer->Colour;
    GlobalRenderer->Colour = Color;
    while(*Char != 0){
        if(*Char == '\n'){
            GlobalRenderer->Next();
            int tmp = (int)TargetFramebuffer->Height/16;
            if(CursorPosition.Y/16 >= tmp-1) ScrollSingleCharUp();
        }
        else if(*Char == '\a'){
            for(int x=0; x<8; x++){
                for(int y=0; y<16; y++){
                    PutPix(CursorPosition.X + x, CursorPosition.Y + y, Colour);
                }
            }
            CursorPosition.X+=8;
            if(CursorPosition.X + 8 > TargetFramebuffer->Width){
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
            }
        }
        else{
            PutChar(*Char, CursorPosition.X, CursorPosition.Y);
            CursorPosition.X+=8;
            if(CursorPosition.X + 8 > TargetFramebuffer->Width){
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
            }
        }
        Char++;
    }
    GlobalRenderer->Colour = OldColor;
}

void BasicRenderer::PutChar(char Char, unsigned int XOffset, unsigned int YOffset)
{
    unsigned int* PixelPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    char* FontPtr = (char*)PSF1Font->GlyphBuffer + (Char * PSF1Font->PSF1Header->CharSize);
    for (unsigned long Y = YOffset; Y < YOffset + 16; Y++){
        for (unsigned long X = XOffset; X < XOffset+8; X++){
            if ((*FontPtr & (0b10000000 >> (X - XOffset))) > 0){
                    *(unsigned int*)(PixelPtr + X + (Y * TargetFramebuffer->PixelsPerScanLine)) = Colour;
                }

        }
        FontPtr++;
    }
}

void BasicRenderer::PutChar(char Char)
{
    PutChar(Char, CursorPosition.X, CursorPosition.Y);
    CursorPosition.X += 8;
    if (CursorPosition.X + 8 > TargetFramebuffer->Width){
        CursorPosition.X = 0; 
        CursorPosition.Y += 16;
    }
}

void BasicRenderer::TickCursor(){
    if(TextCursorShown){
        CursorPosition.X++;
        ClearChar();
        TextCursorShown = false;
    }
    if(!TextCursorShown){
        Print("_");
        CursorPosition.X--;
        TextCursorShown = true;
    }
}

void BasicRenderer::ScrollUp(int lines){
    for(int CurrentLine=0; CurrentLine<lines; CurrentLine++){
        for(int y=1; y<TargetFramebuffer->Height; y++){
            for(int x=0; x<TargetFramebuffer->Width; x++){
                PutPix(x, y-1, GetPix(x, y));
            }
        }
    }
}

void BasicRenderer::ScrollSingleCharUp(){
    for(int y=16; y<TargetFramebuffer->Height; y++){
        for(int x=0; x<TargetFramebuffer->Width; x++){
            PutPix(x, y-16, GetPix(x, y));
        }
    }
    CursorPosition.Y -= 16;
}

void BasicRenderer::ScrollCharUp(int lines){
    for(int i=0; i<lines; i++) ScrollSingleCharUp();
}