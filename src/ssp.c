#include <kernel/features/ssp.h>
#include <stdlib.h>

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((__noreturn__))
void __stack_chk_fail(void)
{
	printf("Stack SMASHING DETECTED\n");
}