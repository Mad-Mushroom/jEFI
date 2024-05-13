#pragma once

#include <stdint.h>
#include "../Graphics/BasicRenderer.h"
#include "../Shell/Shell.h"
#include "../Misc/CStr.h"
#include "Memory/EFIMemory.h"
#include "Memory/Memory.h"
#include "../Graphics/Bitmap.h"
#include "Paging/PageFrameAllocator.h"
#include "Paging/PageMapIndexer.h"
#include "Paging/Paging.h"
#include "Paging/PageTableManager.h"
#include "../UserInput/Mouse.h"
#include "ACPI/ACPI.h"
#include "PCI/PCI.h"

#define KERNEL_VERSION "0.1.1"

struct BootInfo {
	Framebuffer* BootFramebuffer;
	PSF1_FONT* PSF1Font;
	EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor;
	uint64_t MemoryMapSize;
	uint64_t MemoryMapDescriptorSize;
	ACPI::RSDP2* RSDP;
} ;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    PageTableManager* KernelPageTableManager;
};

KernelInfo InitializeKernel(BootInfo* BootInfo);