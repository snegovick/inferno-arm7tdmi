#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#define PRINTF_BUF_SZ 256
#define STDWIDTH 40
#define DONEMSG "[DONE]\n"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#define PRINTF_BUF_SZ 256
#define STDWIDTH 40
#define DONEMSG "[DONE]\n"

#define DATA_SZ 16

int itoa(int n, char s[]);

int printf(const char * fmt, ...);
void printw(char* str);
void l_delay_ms(unsigned int ms);
#endif//__UTIL_H__
