#include "util.h"

char p_buffer[PRINTF_BUF_SZ];
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "util.h"

#include <stdlib.h>
#include <string.h>


//void _outbyte(int c)
void pref_outbyte(int c)
{
  uart0_putc(c);
  /* UART0_Sendchar ((char)c); */
}

void pref_outstr(char *str)
{
  while(*str!=0) {
    pref_outbyte(*str++);
  }
}

#define pref_putchar(c) pref_outbyte(c)

static void pref_printchar(char **str, int c)
{	
  if (str) {
    **str = c;
    ++(*str);
  }
  else (void)pref_putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int pref_prints(char **out, const char *string, int width, int pad)
{
  register int pc = 0, padchar = ' ';

  if (width > 0) {
    register int len = 0;
    register const char *ptr;
    for (ptr = string; *ptr; ++ptr) ++len;
    if (len >= width) width = 0;
    else width -= len;
    if (pad & PAD_ZERO) padchar = '0';
  }
  if (!(pad & PAD_RIGHT)) {
    for ( ; width > 0; --width) {
      pref_printchar (out, padchar);
      ++pc;
    }
  }
  for ( ; *string ; ++string) {
    pref_printchar (out, *string);
    ++pc;
  }
  for ( ; width > 0; --width) {
    pref_printchar (out, padchar);
    ++pc;
  }

  return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 32

static int pref_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
//  uart0_puts("pref 1\r\n");
  char print_buf[PRINT_BUF_LEN];
  char *s;
  int t, neg = 0, pc = 0;
  unsigned int u = i;

//  uart0_puts("pref 2\r\n");
  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return pref_prints (out, print_buf, width, pad);
  }

//  uart0_puts("pref 3\r\n");
  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  }

//  uart0_puts("pref 4\r\n");
  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';

//  uart0_puts("pref 5\r\n");
//  char buf[32];
//  m_itoa(u, buf);
//  uart0_puts(buf);
  while (u) {
    t = u % b;

//    m_itoa(u, buf);
//    uart0_puts("u:");
//    uart0_puts(buf);
//    uart0_puts(" ");    

//    m_itoa(t, buf);
//    uart0_puts("t:");
//    uart0_puts(buf);
//    uart0_puts(" ");    

//    m_itoa(b, buf);
//    uart0_puts("b:");
//    uart0_puts(buf);
//    uart0_puts(" ");    


    if( t >= 10 )
      t += letbase - '0' - 10;

//    m_itoa(t, buf);
//    uart0_puts("t2:");
//    uart0_puts(buf);
//    uart0_puts(" ");    

    *--s = t + '0';
    u = u / b;
//    m_itoa(u, buf);
//    uart0_puts("u2:");
//    uart0_puts(buf);
//    uart0_puts("\r\n");    

  }

//  uart0_puts("pref 6\r\n");
  if (neg) {
    if( width && (pad & PAD_ZERO) ) {
      pref_printchar (out, '-');
      ++pc;
      --width;
    }
    else {
      *--s = '-';
    }
  }
//  uart0_puts("pref 7\r\n");

  return pc + pref_prints (out, s, width, pad);
}


static int pref_print(char **out, const char *format, va_list args )
{
  int width, pad;
  int pc = 0;
  char scr[2];

  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = 0;
      if (*format == '\0') break;
      if (*format == '%') goto out;
      if (*format == '-') {
        ++format;
        pad = PAD_RIGHT;
      }
      while (*format == '0') {
        ++format;
        pad |= PAD_ZERO;
      }
      for ( ; *format >= '0' && *format <= '9'; ++format) {
        width *= 10;
        width += *format - '0';
      }
      if( *format == 's' ) {
        register char *s = (char *)va_arg( args, int );
        pc += pref_prints (out, s?s:"(null)", width, pad);
        continue;
      }
      if( *format == 'd' || *format == 'i') {
        pc += pref_printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
        continue;
      }
      if( *format == 'x' ) {
        pc += pref_printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
        continue;
      }
      if( *format == 'X' ) {
        pc += pref_printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
        continue;
      }
      if( *format == 'u' ) {
        pc += pref_printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
        continue;
      }
      if( *format == 'c' ) {
        /* char are converted to int then pushed on the stack */
        scr[0] = (char)va_arg( args, int );
        scr[1] = '\0';
        pc += pref_prints (out, scr, width, pad);
        continue;
      }
    }
    else {
    out:
      pref_printchar (out, *format);
      ++pc;
    }
  }
  if (out) **out = '\0';
  va_end( args );
  return pc;
}

int pref_printf(const char *format, ...)
{
        va_list args;
        
        va_start( args, format );
        return pref_print( 0, format, args );
}

int pref_sprintf(char *out, const char *format, ...)
{
        va_list args;
        
        va_start( args, format );
        return pref_print( &out, format, args );
}
void pref_printw(char* str)
{
  int width = STDWIDTH;
  char white[STDWIDTH];
  memset(white, '.', STDWIDTH);
  width = width - pref_printf(str);
  white[width+1] = '\0';
  if (width>0)
  {
    pref_outstr(white);
  }
}

int m_strlen(char *s) {
  int len = 0;
  while (*s!='\0') {
    s++;
    len ++;
  }
  return len;
}

//#ifndef __REDLIB__
/* reverse:  переворачиваем строку s на месте */
void m_reverse(char s[])
{
  int i, j;
  char c;
  for (i = 0, j = m_strlen(s) - 1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/* int ntohl(int in) */
/* { */
/*   return in << 24  & 0xFF000000 | in << 8  & 0xFF0000 | in>>8  & 0xFF00 | in>>24 & 0xFF; */
/* } */

/* itoa:  конвертируем n в символы в s */
int m_itoa(int n, char s[])
{
//  n = ntohl(n);
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  m_reverse(s);
  return i;
}
//#endif
