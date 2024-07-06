#include "System/KernelUtil.h"
#include "System/Memory/Heap.h"
#include "System/Scheduling/PIT/PIT.h"

extern "C" void KernelStart(BootInfo* BootInfo){
    KernelInfo KernelInfo = InitializeKernel(BootInfo);
    MainShell->Start();
    //GlobalRenderer->Print("\nPress F1 to start Shell.\nPress F2 to start Graphical User Interface.\n");
    while(true){
        MainKernel->Loop();
    }
}