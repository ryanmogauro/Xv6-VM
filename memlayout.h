// Memory layout

#define EXTMEM  0x100000            // Start of extended memory
#define PHYSTOP 0xE000000           // Top physical memory
#define DEVSPACE 0xFE000000         // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000         // First kernel virtual address
#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#define V2P_ASM(addr) ((addr) - KERNBASE)

#define V2P_WO(x)   ((x) - KERNBASE)
#define P2V_WO(x)   ((x) + KERNBASE)

#ifndef __ASSEMBLER__
typedef unsigned int uintptr_t;
#define V2P(a)  ((uint)((uintptr_t)(a) - KERNBASE))
#define P2V(a)  ((void *)((uintptr_t)(a) + KERNBASE))
#endif



