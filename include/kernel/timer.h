#pragma once
#include "common.h"

#include <kernel/features/ssp.h>

void timer_init();
void handle_timer_1();
void handle_timer_3();
void timer_sleep(u32 ms);
u64 timer_get_ticks();