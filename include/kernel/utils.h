#pragma once
#include "common.h"

void delay(u64);
void put32(u64, unsigned int);
u32 get32(u64 address);
void halt(void);