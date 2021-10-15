#include "stdio.h"
#include "kernel/features/spinlck.h"
#include "kernel/features/arm.h"
#include "kernel/mini_uart.h"
#include <stdint.h>
#include <stdarg.h>

static struct spinlock conslock;
static int panicked = -1;


typedef void (*putcf) (void*,char);
static putcf stdout_putf;
static void* stdout_putp;

#ifdef PRINTF_LONG_SUPPORT

static void uli2a(unsigned long int num, unsigned int base, int uc, char * bf)
{
  int n = 0;
  unsigned int d = 1;
  while(num/d >= base)
    d*=base;
  while(d!=0)
  {
    int dgt = num / d;
    num%=d;
    d/=base;
        
    if (n || dgt>0|| d==0)
    {
      *bf++ = dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10);
      ++n;
    }
  }  
  *bf=0;
}

static void li2a(long num, char * bf)
{
  if (num<0)
  {
    num=-num;
    *bf++ = '-'
  }

  uli2a(num, 10, 0, bf);

}

#endif

static void ui2a(unsigned int num, unsigned int base, int uc, char * bf)
{
  int n=0;
  unsigned int d=1;
  while(num/d >= base)
    d*=base;
  while(d!=0)
  {
    int dgt = num / d;
    num%= d;
    d/=base;
    if(n || dgt>0 || d==0)
    {
      *bf++ = dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10);
      ++n;
    }
  }
  *bf=0;
}

static void i2a (int num, char * bf)
{
  if(num<0)
  {
    num=-num;
    *bf++ = '-';
  }

  ui2a(num, 10, 0, bf);
}

static int a2d(char ch)
{
  if(ch >= '0' && ch <= '9')
    return ch-'0';
  else if(ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  else if(ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  else return -1;
}

static char a2i(char ch, char** src, int base, int* nump)
{
  char* p = *src;
  int num = 0;
  int digit;

  while((digit=a2d(ch))>=0)
  {
    if(digit > base)
    {
      break;
    }

    num = num*base+digit;
    ch = *p++;
  }

  *src = p;
  *nump = num;
  return ch;
}

static void putchw(void* putp, putcf putf, int n, char z, char* bf)
{
  char fc = z? '0' : ' ';
  char ch;
  char* p = bf;

  while(*p++ && n > 0)
    n--;

  while(n-- > 0)
    putf(putp, fc);

  while((ch = *bf++))
    putf(putp, ch);
}

void tfp_format(void* putp, putcf putf, char *fmt, va_list va)
{
  char bf[12];
  
  char ch;

  while((ch=*(fmt++)))
  {
    if(ch != '%')
        putf(putp, ch);
    else
    {
      char lz = 0;

#ifdef PRINTF_LONG_SUPPORT
      char lng = 0;
#endif

      int w = 0;
      ch = *(fmt++);

      if(ch == '0')
      {
        ch = * (fmt++);
        lz = 1;
      }

      if(ch >= '0' && ch <= '9')
      {
        ch = a2i(ch, &fmt, 10, &w);
      }

#ifdef PRINTF_LONG_SUPPORT
      if(ch == 'l')
      {
        ch = *(fmt++);
        lng = 1;
      }
#endif

      switch(ch)
      {
        case 0:
            goto abort;
        case 'u': {
#ifdef PRINTF_LONG_SUPPORT
            if(lng)
                uli2a(va_arg(va, unsigned long int), 10, 0, bf);
            else
#endif
              ui2a(va_arg(va, unsigned int), 10, 0, bf);
              putchw(putp, putf, w, lz, bf);
              break;
          }

          case 'd': 
          {
#ifdef PRINTF_LONG_SUPPORT
            if(lng)
                li2a(va_arg(va, unsigned long int), bf);
            else
#endif    
            i2a(va_arg(va, int), bf);
            putchw(putp, putf, w, lz, bf);
            break;
          }

          case 'x': case 'X':
#ifdef PRINTF_LONG_SUPPORT
              if (lng)
                  ui2a(va_arg(va, unsigned long int), 16, (ch == 'X'), bf);
              else
#endif        
              ui2a(va_arg(va, unsigned int), 16, (ch == 'X'), bf);
              putchw(putp, putf, w, lz, bf);
              break;
            
            case 'c':
                putf(putp, (char)(va_arg(va, int)));
                break;
            case 's':
                putchw(putp, putf, w, 0, va_arg(va, char*));
                break;
            case '%':
                putf(putp, ch);
            default:
                break;
        }
      }
    }

abort:;
}

void init_printf(void* putp, void (*putf) (void*,char))
{
  stdout_putf = putf;
  stdout_putp = putp;
}

void tfp_printf(char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  tfp_format(stdout_putp, stdout_putf, fmt, va);
  va_end(va);
}

static void putcp(void* p, char c)
{
  *(*((char**)p))++ = c;
}

void tfp_sprintf(char* s, char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  tfp_format(&s, putcp, fmt, va);
  putcp(&s, 0);
  va_end(va);
}

static void
printint(int64_t x, int base, int sign)
{
    static char digit[] = "0123456789abcdef";
    static char buf[64];

    if (sign && x < 0) {
        x = -x;
        uart_send('-');
    }

    int i = 0;
    uint64_t t = x;
    do {
        buf[i++] = digit[t % base];
    } while (t /= base);

    while (i--) uart_send(buf[i]);
}

void
vprintfmt(void (*putch)(int), const char *fmt, va_list ap)
{
    int i, c;
    char *s;
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            putch(c);
            continue;
        }

        int l = 0;
        for (; fmt[i+1] == 'l'; i++)
            l++;

        if (!(c = fmt[++i] & 0xff))
            break;

        switch (c) {
        case 'u':
            if (l == 2) printint(va_arg(ap, int64_t), 10, 0);
            else printint(va_arg(ap, uint32_t), 10, 0);
            break;
        case 'd':
            if (l == 2) printint(va_arg(ap, int64_t), 10, 1);
            else printint(va_arg(ap, int), 10, 1);
            break;
        case 'x':
            if (l == 2) printint(va_arg(ap, int64_t), 16, 0);
            else printint(va_arg(ap, uint32_t), 16, 0);
            break;
        case 'p':
            printint((uint64_t)va_arg(ap, void *), 16, 0);
            break;
        case 'c':
            putch(va_arg(ap, int));
            break;
        case 's':
            if ((s = (char*)va_arg(ap, char *)) == 0)
                s = "(null)";
            for (; *s; s++)
                putch(*s);
            break;
        case '%':
            putch('%');
            break;
        default:
            /* Print unknown % sequence to draw attention. */
            putch('%');
            putch(c);
            break;
        }
    }
}

void
cprintf(const char *fmt, ...)
{
    va_list ap;

    acquire(&conslock);
    if (panicked >= 0 && panicked != cpuid()) {
        release(&conslock);
        while (1) ;
    }
    va_start(ap, fmt);
    vprintfmt(uart_send, fmt, ap);
    va_end(ap);
    release(&conslock);
}

void
kpanic(const char *fmt, ...)
{
    va_list ap;

    acquire(&conslock);
    if (panicked < 0) panicked = cpuid();
    else {
        release(&conslock);
        while (1) ;
    }
    va_start(ap, fmt);
    vprintfmt(uart_send, fmt, ap);
    va_end(ap);
    release(&conslock);

    cprintf("%s:%d: kernel panic at cpu %d.\n", __FILE__, __LINE__, cpuid());
    while (1) ;
}