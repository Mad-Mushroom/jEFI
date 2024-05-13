#include "System/KernelUtil.h"
#include "System/Memory/Heap.h"
#include "System/Scheduling/PIT/PIT.h"

extern "C" void KernelStart(BootInfo* BootInfo){
    KernelInfo KernelInfo = InitializeKernel(BootInfo);
    MainShell->Start();
    MainShell->Print("\n");
    while(true){
        asm ("hlt");
    }
}