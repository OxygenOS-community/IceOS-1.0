#pragma once

#include <stdint.h>
#include <stdlib.h>
 
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

__attribute__((__noreturn__))
void __stack_chk_fail(void);