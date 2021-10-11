#include "kernel/drivers/led.h"
#include "kernel/drivers/spi.h"

const static u8 DIGIT_TABLE [] = {
	0b01111110,0b00110000,0b01101101,0b01111001,0b00110011,0b01011011,0b01011111,0b01110000,0b01111111,0b01111011
};

void led_init() {
	led_send_cmd(LD_SCANLIMIT, 7);
	led_send_cmd(LD_DECODE_MODE, 0);
	led_send_cmd(LD_SHUTDOWN, 1);
	led_send_cmd(LD_INTENSITY, 0);
	led_send_cmd(LD_DISPLAY_TEST, 0);
	led_intensity(5);
}

void led_send_cmd(u8 opcode, u8 data) {
	u8 cmd[2];
	cmd[0] = opcode;
	cmd[1] = data;

	spi_send(0, cmd, 2);
}

void led_clear() {
	for (int i=LD_DIGIT0; i<=LD_DIGIT7; i++) {
		led_send_cmd(i, 0);
	}
}

void led_intensity(u8 value) {
	led_send_cmd(LD_INTENSITY, value);
}

void led_set_digit(u8 digit, u8 value, bool dot) {
	u8 digit_value = DIGIT_TABLE[value];

	if(dot) {
		digit_value |= 0x80;
	}

	led_send_cmd(LD_DIGIT0 + digit, digit_value);
}