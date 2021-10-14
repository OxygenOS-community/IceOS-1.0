#pragma once
#include <kernel/perip/gpio.h>

#include <kernel/features/ssp.h>

typedef enum _GpioFunc
{
    GFInput = 0,
    GFOutput = 1,
    GFAlt0 = 4,
    GFAlt1 = 5,
    GFAlt2 = 6,
    GFAlt3 = 7,
    GFAlt4 = 3,
    GFAlt5 = 2
} GpioFunc;

void gpio_pin_enable(u8 pinNumber);
void gpio_pin_set_func(u8 pinNumber, GpioFunc func);
void gpio_useAsAlt3(unsigned int pin_number);
void uart_hex(unsigned int d);
