#pragma once

#include <sys/cdefs.h>
#include <kernel/features/ssp.h>

int compare(const char* X, const char* Y);

__attribute__((noreturn))
void abort(void);
void panic(void);