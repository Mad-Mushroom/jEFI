#include "Panic.h"
#include "../Graphics/BasicRenderer.h"

void Panic(const char* PanicMessage){
    GlobalRenderer->ClearColour = 0x00ff0000;
    GlobalRenderer->Clear();

    GlobalRenderer->CursorPosition = {0, 0};

    GlobalRenderer->Colour = 0xffffffff;

    GlobalRenderer->Print("Kernel Panic!");

    GlobalRenderer->Next();
    GlobalRenderer->Next();

    GlobalRenderer->Print(PanicMessage);
}