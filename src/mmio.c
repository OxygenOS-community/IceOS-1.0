#include "kernel/mmio.h"
#include "kernel/common.h"
#include "stdlib.h"
#include "stdio.h"

void mmio_init(int raspi) {
	switch (raspi) {
		case 3: MMIO_BASE = 0x3F000000; break;
		case 4: MMIO_BASE = 0xFE000000; break;
		default: printf("PANIC: INVAILD RPI MMIO ADDR"); panic(); break;
	}
}

void mmio_write(u32 reg, u32 data) {
	*(volatile u32*)(MMIO_BASE + reg) = data;
}

u8 mmio_read(u32 reg) {
	return *(volatile u32*)(MMIO_BASE + reg);
}