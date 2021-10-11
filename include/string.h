#pragma once

#include "kernel/common.h"
#include <stddef.h>

void *memcpy(void *dst, const void *src, u32 n);
void *memmove(void*, const void*, size_t);