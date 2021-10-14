#include "stdio.h"
#include "kernel/mini_uart.h"

void putc(void *p, char c)
{
  if(c == '\n')
  {	
    uart_send('\r');
  }

  uart_send(c);
}