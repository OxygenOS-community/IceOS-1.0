#include "stdlib.h"

__attribute__((noreturn))
void abort(void) {
	printf("Abort() was called, exiting..\n");
	while(1){}
	__builtin_unreachable();
}