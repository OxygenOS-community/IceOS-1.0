#pragma once

#include <kernel/common.h>
#include <kernel/features/ssp.h>

void lcd_init(u8 address);

void lcd_backlight(bool on);

void lcd_print(char *s);

void lcd_send(u8 data, u8 mode);

void lcd_command(u8 command);