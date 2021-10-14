#ifndef __STDIO_H
#define __STDIO_H 1
#include <stdarg.h>
#include <kernel/features/ssp.h>

void init_printf(void* putp, void (*putf) (void*,char));

void tfp_printf(char *fmt, ...);
void tfp_sprintf(char* s, char *fmt, ...);
void tfp_format(void* putp, void (*putf) (void*,char), char *fmt, va_list va);

void putc(void *p, char c);
int puts(const char*);

#define printf tfp_printf
#define sprintf tfp_sprintf
#define assert(x)  { if (!(x)) { printf("%s:%d: assertion failed.\n", __FILE__, __LINE__); while(1); }  }


#endif