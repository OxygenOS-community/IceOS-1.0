#ifndef _KERNEL_PERIP_GPIO_H
#define _KERNEL_PERIP_GPIO_H
#include "kernel/common.h"
#include "kernel/perip/base.h"
#include <kernel/features/ssp.h>
#include <kernel/mmio.h>

#define GPIO_BASE       (MMIO_BASE + 0x200000),
#define GPFSEL1         (GPIO_BASE + 0x04)
#define GPFSEL2         (GPIO_BASE + 0x08)
#define GPFSEL3         (GPIO_BASE + 0x0C)
#define GPFSEL4         (GPIO_BASE + 0x10)
#define GPFSEL5         (GPIO_BASE + 0x14)
#define GPSET1          (GPIO_BASE + 0x20)
#define GPLEV0          (GPIO_BASE + 0x34)
#define GPLEV1          (GPIO_BASE + 0x38)
#define GPEDS1          (GPIO_BASE + 0x44)
#define GPHEN1          (GPIO_BASE + 0x68)
#define GPPUD           (GPIO_BASE + 0x94)
#define GPPUDCLK0       (GPIO_BASE + 0x98)
#define GPPUDCLK1       (GPIO_BASE + 0x9C)


struct GpioPinData
{
    reg32 reserved;
    reg32 data[2];
};

struct GpioRegs
{
    reg32 func_select[6];
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData level;
    struct GpioPinData ev_detect_status;
    struct GpioPinData re_detect_enable;
    struct GpioPinData fe_detect_enable;
    struct GpioPinData hi_detect_enable;
    struct GpioPinData lo_detect_enable;
    struct GpioPinData async_re_detect;
    struct GpioPinData async_fe_detect;
    reg32 reserved;
    reg32 pupd_enable;
    reg32 pupd_enable_clocks[2];
};

#define REGS_GPIO ((struct GpioRegs *)(PBASE + 0x0020000))


#endif