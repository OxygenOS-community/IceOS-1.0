#pragma once

#include "kernel/common.h"
#include <stddef.h>
#include "stdio.h"
#include <kernel/features/ssp.h>

void *memcpy(void *dst, const void *src, u32 n);
void *memmove(void*, const void*, size_t);
int memcmp(const char *, const char *, int count);
size_t strlen(const char* str);
char* strcat(char* dst, const char* src);
int strcmp(const char* x, const char* y);
const char* strstr(const char* X, const char* Y);