#include "kernel/utils.h"
#include "stdlib.h"

__attribute__((noreturn))
void panic(void) {
	//TODO: add kernel panic
	printf("ERROR: PANIC OCCURED, SYSTEM HALTED\n");
	halt();
}