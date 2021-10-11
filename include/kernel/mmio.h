#pragma once

#include <kernel/common.h>

u32 MMIO_BASE;

inline void mmio_init(int raspi);

inline void mmio_write(u32 reg, u32 data);

inline void mmio_read(u32 reg);