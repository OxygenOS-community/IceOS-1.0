#pragma once
#include "kernel/perip/base.h"
#include "kernel/common.h"

#define CLOCKHZ 1000000

struct timer_regs {
  reg32 control_status;
  reg32 counter_lo;
  reg32 counter_hi;
  reg32 compare[4];
};

#define REGS_TIMER ((struct timer_regs *)(PBASE + 0x00003000))