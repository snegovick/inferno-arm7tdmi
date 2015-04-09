#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "bsp_regs.h"

void uart0_init (void);
void uart0_putc(uint16_t c);
void uart0_puts(char *s);
void uart0_putn(char *s, unsigned int n);
void uart0_addr(void *a);

#define uart0_put(s) {uart0_putn(s, sizeof(s));}

#endif/*__BSP_UART_H__*/
