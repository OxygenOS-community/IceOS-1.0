#pragma once

#define PHYSTOP 0xF000000            // Top physical memory
// #define DEVSPACE 0xFE000000        // Other devices are at high addresses

#define KERNBASE 0xFFFF000000000000   // First kernel virtual address
#define KERNLINK (KERNBASE+0x80000)   // Address where kernel is linked

#ifndef __ASSEMBLER__

#include <stdint.h>
#define V2P(a) (((uint64_t) (a)) - KERNBASE)
#define P2V(a) ((void *)(((char *) (a)) + KERNBASE))

#endif  /* !__ASSEMBLER__ */

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts
