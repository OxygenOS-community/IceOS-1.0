#pragma once

#include <kernel/common.h>

void led_init();

void led_send_cmd(u8 opcode, u8 data);

void led_clear();

void led_intensity(u8 value);

void led_set_digit(u8 digit, u8 value, bool dot);

void led_set_val(int value);

typedef enum {
	LD_NOOP = 0,
	LD_DIGIT0,
	LD_DIGIT1,
	LD_DIGIT2,
	LD_DIGIT3,
	LD_DIGIT4,
	LD_DIGIT5,
	LD_DIGIT6,
	LD_DIGIT7,
	LD_DECODE_MODE,
	LD_INTENSITY,
	LD_SCANLIMIT,
	LD_SHUTDOWN,
	LD_DISPLAY_TEST = 15,
} led_opcodes;