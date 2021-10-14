#include "kernel/utils.h"
#include "stdlib.h"
#include "stdio.h"

__attribute__((noreturn))
void panic(void) {
	//TODO: add kernel panic
	printf("ERROR: PANIC CALLED");
	halt();
}