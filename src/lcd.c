#include "kernel/drivers/lcd.h"
#include "kernel/drivers/i2c.h"
#include "kernel/timer.h"
#include "stdio.h"

typedef enum Flags {
	FLAG_RS = 1,
	FLAG_RW = 2,
	FLAG_EN = 4
};

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_BACKLIGHT 8
#define LCD_NOBACKLIGHT 0

static u8 _backlight = LCD_BACKLIGHT;
static u8 _lcd_address = 0;

static void write_i2c(u8 data)
{
	u8 value = data | _backlight;
	i2c_send(_lcd_address, &value, 1);
}

static void pulse(u8 data)
{
	write_i2c(data | FLAG_EN);
	timer_sleep(5);

	write_i2c(data & ~FLAG_EN);
	timer_sleep(1);
}

static void write_4bits(u8 data)
{
	write_i2c(data);
	pulse(data);
}

void lcd_send(u8 data, u8 mode)
{
	write_4bits((data & 0xF0) | mode);
	write_4bits(((data << 4) & 0xF0) | mode);
}

void lcd_command(u8 command)
{
	lcd_send(command, 0);
}

void lcd_init(u8 address)
{
	_lcd_address = address;

	write_4bits(3 << 4);
	timer_sleep(5);
	write_4bits(3 << 4);
	timer_sleep(1);
	write_4bits(3 << 4);
	write_4bits(2 << 4);
	
	lcd_command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
	lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
	lcd_command(LCD_CLEARDISPLAY);
	timer_sleep(2);

	lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);

	lcd_command(LCD_RETURNHOME);
	timer_sleep(2);
}

void lcd_backlight(bool on)
{
	if(on)
	{
		_backlight = LCD_BACKLIGHT;
	}
	else
	{
		_backlight = LCD_NOBACKLIGHT;
	}
}

void lcd_print(char *s)
{
	while(*s)
	{
		lcd_send(*s, FLAG_RS);
		s++;
	}
}