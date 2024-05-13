#include "Shell.h"

jShell* MainShell;

jShell::jShell(){
    this->IsRightShiftPressed = false;
    this->IsLeftShiftPressed = false;
    this->CommandBufferSize = 0;
    CursorPosition.X = 0;
    CursorPosition.Y = 16;
}

void jShell::DrawTitlebar(){
    for(int i=0; i<GlobalRenderer->TargetFramebuffer->Width/8; i++){
        GlobalRenderer->Print("\a", COLOR_BLUE);
    }
    GlobalRenderer->CursorPosition = {0, 0};
    GlobalRenderer->Print("The J Operating System", COLOR_YELLOW);
}

void jShell::Start(){
    GlobalRenderer->Clear();
    GlobalRenderer->ClearColour = COLOR_LIGHT_GRAY;
    this->DrawTitlebar();
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    for(int i=0; i<GlobalRenderer->TargetFramebuffer->Width/8; i++){
        GlobalRenderer->Print("\a", COLOR_LIGHT_GRAY);
    }
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    GlobalRenderer->Colour = COLOR_BLACK;
    GlobalRenderer->Print("jShell> ", COLOR_BLACK);
}

void jShell::RunShell(){
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    for(int i=0; i<GlobalRenderer->TargetFramebuffer->Width/8; i++){
        GlobalRenderer->Print("\a", COLOR_LIGHT_GRAY);
    }
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    GlobalRenderer->Colour = COLOR_BLACK;
    GlobalRenderer->Print("jShell> ", COLOR_BLACK);
}

void jShell::Print(const char* String, uint32_t Color){
    char* Char = (char*)String;
    unsigned int OldColor = GlobalRenderer->Colour;
    GlobalRenderer->Colour = Color;
    while(*Char != 0){
        if(*Char == '\n'){
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }
        else if(*Char == '\a'){
            for(int x=0; x<8; x++){
                for(int y=0; y<16; y++){
                    GlobalRenderer->PutPix(CursorPosition.X + x, CursorPosition.Y + y, Color);
                }
            }
            CursorPosition.X+=8;
            if(CursorPosition.X + 8 > GlobalRenderer->TargetFramebuffer->Width){
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
            }
        }
        else{
            GlobalRenderer->PutChar(*Char, CursorPosition.X, CursorPosition.Y);
            CursorPosition.X+=8;
            if(CursorPosition.X + 8 > GlobalRenderer->TargetFramebuffer->Width){
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
            }
        }
        Char++;
    }
    GlobalRenderer->Colour = OldColor;
}

void jShell::Clear(uint32_t Color){
    uint32_t OldClearColor = GlobalRenderer->ClearColour;
    GlobalRenderer->ClearColour = Color;
    GlobalRenderer->Clear();
    DrawTitlebar();
    GlobalRenderer->ClearColour = OldClearColor;
    CursorPosition = {0, 0};
    Print("\n");
}

void jShell::ParseCommand(){
    index = 0;
    index2 = 0;
    args = 0;
    for(int i=0; i<64; i++){
        for(int j=0; j<256; j++){
            cmd_argv[i][j] = 0;
        }
    }

    for(int i=0; i<CommandBufferSize; i++){
        if(CommandBuffer[index] == ' '){ args++; index++; index2 = 0; }
        cmd_argv[args][index2] = CommandBuffer[index];
        index++;
        index2++;
    }

    if(strcmp(cmd_argv[0], "echo")){
        for(int i=1; i<15; i++){
            Print(cmd_argv[i]);
            Print(" ");
        }
        //PrintString("\n");
        //Shell_bufferSize = 0; memset(arguments, 0, sizeof(arguments));
        //return;
    }
    Print("Index: ");
    Print(ToString(index));
    Print("\nIndex2: ");
    Print(ToString(index2));
    Print("\nArgs: ");
    Print(ToString(args));
    Print("\nCmd_Argv0: ");
    Print(cmd_argv[0]);
    Print("\nCmd_Argv1: ");
    Print(cmd_argv[1]);
}

void jShell::HandleKeyPress(uint8_t Scancode){
    switch (Scancode){
        case LeftShift:
            this->IsLeftShiftPressed = true;
            return;
        case LeftShift + 0x80:
            this->IsLeftShiftPressed = false;
            return;
        case RightShift:
            this->IsRightShiftPressed = true;
            return;
        case RightShift + 0x80:
            this->IsRightShiftPressed = false;
            return;
        case Enter:
            Clear();
            ParseCommand();
            RunShell();
            return;
        case Spacebar:
            GlobalRenderer->PutChar(' ');
            CommandBuffer[CommandBufferSize] = ' ';
            CommandBufferSize++;
            return;
        case BackSpace:
           if(CommandBufferSize > 0){
                GlobalRenderer->ClearChar();
                CommandBufferSize--;
                CommandBuffer[CommandBufferSize] = 0;
           }
           return;
    }

    char ASCII = QWERTYKeyboard::Translate(Scancode, IsLeftShiftPressed | IsRightShiftPressed);

    if (ASCII != 0){
        GlobalRenderer->PutChar(ASCII);
        CommandBuffer[CommandBufferSize] = ASCII;
        CommandBufferSize++;
    }
}