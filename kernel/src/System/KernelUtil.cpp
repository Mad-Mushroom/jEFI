#include "KernelUtil.h"
#include "GDT/GDT.h"
#include "../Interrupts/IDT.h"
#include "../Interrupts/Interrupts.h"
#include "IO.h"
#include "Memory/Heap.h"

KernelInfo kernelInfo; 

void PrepareMemory(BootInfo* BootInfo){
    uint64_t mMapEntries = BootInfo->MemoryMapSize / BootInfo->MemoryMapDescriptorSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(BootInfo->MemoryMapFirstDescriptor, BootInfo->MemoryMapSize, BootInfo->MemoryMapDescriptorSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    Memset(PML4, 0, 0x1000);

    GlobalPageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(BootInfo->MemoryMapFirstDescriptor, mMapEntries, BootInfo->MemoryMapDescriptorSize); t+= 0x1000){
        GlobalPageTableManager.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)BootInfo->BootFramebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)BootInfo->BootFramebuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        GlobalPageTableManager.MapMemory((void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));

    kernelInfo.KernelPageTableManager = &GlobalPageTableManager;
}

IDTR IDTR;
void SetIDTGate(void* Handler, uint8_t EntryOffset, uint8_t TypeAttributes, uint8_t Selector){

    IDTDescEntry* Interrupt = (IDTDescEntry*)(IDTR.Offset + EntryOffset * sizeof(IDTDescEntry));
    Interrupt->SetOffset((uint64_t)Handler);
    Interrupt->TypeAttributes = TypeAttributes;
    Interrupt->Selector = Selector;
}

void PrepareInterrupts(){
    IDTR.Limit = 0x0FFF;
    IDTR.Offset = (uint64_t)GlobalAllocator.RequestPage();

    SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);
 
    asm ("lidt %0" : : "m" (IDTR));

    RemapPIC();
}

void PrepareACPI(BootInfo* BootInfo){
    ACPI::SDTHeader* XSDT = (ACPI::SDTHeader*)(BootInfo->RSDP->XSDTAddress);
    
    ACPI::MCFGHeader* MCFG = (ACPI::MCFGHeader*)ACPI::FindTable(XSDT, (char*)"MCFG");

    PCI::EnumeratePCI(MCFG);
}

void StartupStatusMessage(const char* Type, const char* Text, int Status){
    GlobalRenderer->Print("[ "); GlobalRenderer->Print(Type, COLOR_GRAY); GlobalRenderer->Print(" ] "); GlobalRenderer->Print(Text);
    if(Status >= 0){
        if(Status == 0){
            GlobalRenderer->CursorPosition.X = (GlobalRenderer->TargetFramebuffer->Width - 80);
            GlobalRenderer->Print(" [  "); GlobalRenderer->Print("OK", COLOR_GREEN); GlobalRenderer->Print("  ]\n");
        }
        if(Status == 1){
            GlobalRenderer->CursorPosition.X = (GlobalRenderer->TargetFramebuffer->Width - 80);
            GlobalRenderer->Print(" [ "); GlobalRenderer->Print("ERROR", COLOR_RED); GlobalRenderer->Print(" ]");
        }
    }
}

BasicRenderer r = BasicRenderer(NULL, NULL);
jShell s = jShell();
KernelInfo InitializeKernel(BootInfo* BootInfo){
    r = BasicRenderer(BootInfo->BootFramebuffer, BootInfo->PSF1Font);
    GlobalRenderer = &r; GlobalRenderer->Clear();
    
    GDTDescriptor GDTDescriptor;
    GDTDescriptor.Size = sizeof(GDT) - 1;
    GDTDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&GDTDescriptor);
    StartupStatusMessage("INFO", "GDT initialized.", 0);

    PrepareMemory(BootInfo);
    StartupStatusMessage("INFO", "Memory initialized.", 0);

    InitializeHeap((void*)0x0000100000000000, 0x10);
    StartupStatusMessage("INFO", "Heap initialized.", 0);

    PrepareInterrupts();
    StartupStatusMessage("INFO", "IDT initialized.", 0);

    InitPS2Mouse();
    StartupStatusMessage("INFO", "PS/2 Mouse initialized.", 0);

    //PrepareACPI(BootInfo); // Not working!
    StartupStatusMessage("INFO", "ACPI initialized.", 1);

    OutByte(PIC1_DATA, 0b11111000);
    OutByte(PIC2_DATA, 0b11101111);
    StartupStatusMessage("INFO", "PIC initialized.", 0);

    asm ("sti");

    s = jShell();
    MainShell = &s;
    StartupStatusMessage("INFO", "Shell initialized.", 0);

    StartupStatusMessage("INFO", "Done.", -1);

    GlobalRenderer->Print("\n\nWelcome to "); GlobalRenderer->Print("j", COLOR_LIGHT_BLUE); GlobalRenderer->Print("OS", COLOR_BLUE); GlobalRenderer->Print("!\n\n");
    GlobalRenderer->Print("jOS Version "); GlobalRenderer->Print(KERNEL_VERSION);
    GlobalRenderer->Print("\nLicensed under GPL3; For more infos, type 'license';\n<https://www.gnu.org/licenses/gpl-3.0.en.html>\n");

    return kernelInfo;
}