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
    GlobalRenderer->ClearColour = COLOR_BLACK;
    GlobalRenderer->Clear();
    CursorPosition = {0, 0};
    MainKernel->BootState = 4;
    //GlobalRenderer->ClearColour = COLOR_DARK_GRAY;
    this->DrawTitlebar();
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    for(int i=0; i<GlobalRenderer->TargetFramebuffer->Width/8; i++){
        GlobalRenderer->Print("\a", COLOR_DARK_GRAY);
    }
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    GlobalRenderer->Colour = COLOR_WHITE;
    GlobalRenderer->ClearColour = COLOR_DARK_GRAY;
    GlobalRenderer->Print("jShell> ", COLOR_WHITE);
    Print("\njOS ", COLOR_LIGHT_BLUE); Print(OS_VERSION);
    Print("\njKern ", COLOR_LIGHT_BLUE); Print(KERNEL_VERSION);
    Print("\njShell ", COLOR_LIGHT_BLUE); Print(SHELL_VERSION);
}

void jShell::RunShell(){
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    for(int i=0; i<GlobalRenderer->TargetFramebuffer->Width/8; i++){
        GlobalRenderer->Print("\a", COLOR_DARK_GRAY);
    }
    GlobalRenderer->CursorPosition = {0, GlobalRenderer->TargetFramebuffer->Height-16};
    GlobalRenderer->Colour = COLOR_WHITE;
    GlobalRenderer->ClearColour = COLOR_DARK_GRAY;
    GlobalRenderer->Print("jShell> ", COLOR_WHITE);
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

void jShell::PutChar(char chr, uint32_t Color){
    unsigned int OldColor = GlobalRenderer->Colour;
    GlobalRenderer->Colour = Color;
    Point OldCursorPos = GlobalRenderer->CursorPosition;
    GlobalRenderer->CursorPosition = CursorPosition;
    GlobalRenderer->PutChar(chr);
    CursorPosition.X+=8;
    if(CursorPosition.X + 8 > GlobalRenderer->TargetFramebuffer->Width){
        CursorPosition.X = 0;
        CursorPosition.Y += 16;
    }
    GlobalRenderer->CursorPosition = OldCursorPos;
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

void jShell::DisplayInfo(){
    unsigned int u_hours = 0;
        unsigned int u_mins = 0;
        unsigned int u_secs = 0;
        u_mins = (int)PIT::TimeSinceBoot/100/60;
        u_hours = (int)PIT::TimeSinceBoot/100/60/60;
        u_secs = (int)PIT::TimeSinceBoot/100 - (u_mins*60) - (u_hours*60*60);
        Print("OS: The J Operating System "); Print(OS_VERSION);
        Print("\nKernel: jKern "); Print(KERNEL_VERSION);
        Print("\nUptime: "); if(u_hours > 0){ Print(ToString(u_hours)); Print(" hours, "); } if(u_mins > 0){ Print(ToString(u_mins)); Print(" mins, "); } Print(ToString(u_secs)); Print(" secs");
        Print("\nShell: jShell "); Print(SHELL_VERSION);
        Print("\nResolution: "); Print(ToString(GlobalRenderer->TargetFramebuffer->Width)); Print("x"); Print(ToString(GlobalRenderer->TargetFramebuffer->Height));
        Print("\n\n");
        Print("\a\a", COLOR_BLACK); Print("\a\a", COLOR_CYAN); Print("\a\a", COLOR_LIGHT_RED);
        Print("\a\a", COLOR_DARK_GRAY); Print("\a\a", COLOR_MAGENTA); Print("\a\a", COLOR_BLUE);
        Print("\a\a", COLOR_GRAY); Print("\a\a", COLOR_YELLOW); Print("\a\a", COLOR_GREEN);
        Print("\a\a", COLOR_LIGHT_GRAY); Print("\a\a", COLOR_LIGHT_BLUE); Print("\a\a", COLOR_RED);
        Print("\a\a", COLOR_BROWN); Print("\a\a", COLOR_LIGHT_GREEN); Print("\a\a", COLOR_WHITE);
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
    }

    else if(strcmp(cmd_argv[0], "info")){
        DisplayInfo();
    }

    else if(strcmp(cmd_argv[0], "clear")){
        Clear();
    }

    else if(strcmp(cmd_argv[0], "dahci")){
        uint8_t* buff = GlobalAHCI->ReadPort(atoi(cmd_argv[1]), atoi(cmd_argv[2]));
        for(int i=0; i<atoi(cmd_argv[2]); i++){
            PutChar(buff[i]);
        }
    }
    else if(strcmp(cmd_argv[0], "dpci")){
        Print("PCI");
    }


    else if(!strcmp(cmd_argv[0], "")){
        Print("Don't know what you mean...");
    }

    Memset(&CommandBuffer, 0, sizeof(CommandBuffer));
    CommandBufferSize = 0;
    Memset(&cmd_argv, 0, sizeof(cmd_argv));
}

void jShell::HandleKeyPress(uint8_t Scancode){
    if(MainKernel->BootState < 3) return;
    switch (Scancode){
        case KEY_F1:
            //Start();
            return;

        case KEY_F2:
            Clear();
            Print("jUI is under development.\n\nPlease be patient :)");
            RunShell();
            return;

        if(MainKernel->BootState < 4) return;

        case KEY_LEFT_SHIFT:
            this->IsLeftShiftPressed = true;
            return;
        case KEY_LEFT_SHIFT + 0x80:
            this->IsLeftShiftPressed = false;
            return;
        case KEY_RIGHT_SHIFT:
            this->IsRightShiftPressed = true;
            return;
        case KEY_RIGHT_SHIFT + 0x80:
            this->IsRightShiftPressed = false;
            return;
        case KEY_ENTER:
            Clear();
            ParseCommand();
            RunShell();
            return;
        case KEY_SPACEBAR:
            GlobalRenderer->PutChar(' ');
            CommandBuffer[CommandBufferSize] = ' ';
            CommandBufferSize++;
            return;
        case KEY_BACKSPACE:
           if(CommandBufferSize > 0){
                GlobalRenderer->ClearChar();
                CommandBufferSize--;
                CommandBuffer[CommandBufferSize] = 0;
           }
           return;

        case KEY_F12:
            Clear();
            DisplayInfo();
            RunShell();
            return;
    }

    if(MainKernel->BootState < 4) return;
    char ASCII = QWERTYKeyboard::Translate(Scancode, IsLeftShiftPressed | IsRightShiftPressed);

    if (ASCII != 0){
        GlobalRenderer->PutChar(ASCII);
        CommandBuffer[CommandBufferSize] = ASCII;
        CommandBufferSize++;
    }
}