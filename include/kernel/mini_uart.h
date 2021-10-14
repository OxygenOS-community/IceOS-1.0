#pragma once

#include <kernel/features/ssp.h>

void uart_init();
char uart_recv();
void uart_send(char c);
void uart_send_string(char *str);
void uart_hex(unsigned int d);