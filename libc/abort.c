#include "stdlib.h"
#include "stdio.h"

__attribute__((noreturn))
void abort(void) {
	printf("Abort() was called, exiting..\n");
	while(1){}
	__builtin_unreachable();
}