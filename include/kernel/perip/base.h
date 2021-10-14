#pragma once

#include <kernel/features/ssp.h>

#if RPI_VERSION == 3
#define PBASE 0x3F00000


#elif RPI_VERSION == 4
#define PBASE 0xFE000000

#else
#define PBASE 0
#error RPI_VERSION NOT DEFINED

#endif

#define CORE_CLOCK_SPEED 1500000000