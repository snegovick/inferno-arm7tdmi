#include "bsp_regs.h"
#include "bsp_uart.h"

void uart0_init (void) {
  BSP_PCONP |= BSP_PCONP_UART0;
  //leave clock by default
  BSP_PCLK_SEL0 &= ~BSP_PCLK_SEL0_UART0_11;

  //enable dll/dlm regs
  BSP_U0LCR |= BSP_U0LCR_DLAB;

  // set baudrate
  BSP_U0DLM = 0;
  BSP_U0DLL = 5;
  BSP_U0FDR |= (BSP_U0FDR_DIVADDVAL(5) | BSP_U0FDR_MULVAL(9));

  // enable FIFO
  BSP_U0FCR |= BSP_U0FCR_FIFO;

  // set pins
  BSP_PINSEL0 |= (BSP_PINSEL0_UART0_P02 | BSP_PINSEL0_UART0_P03);

  // usnet DLAB
  BSP_U0LCR &= ~BSP_U0LCR_DLAB;
}

void uart0_putc(uint16_t c) {
  int retries = 100;
  //for (;retries>0;retries--) {
  for (;;) {
    if (BSP_U0LSR & BSP_U0LSR_THRE) {
      break;
    }
  }
  if (retries == 0) {
    return;
  }
  BSP_U0THR = c & 0xFF;
}

void uart0_puts(char *s) {
  int i = 0;
  while (s[i] != 0) {
    //uart0_addr(&s[i]);
    //uart0_putc(' ');

    uart0_putc(s[i]);
    //uart0_putc(' ');
    //uart0_putc('\r');
    //uart0_putc('\n');
    i++;
  }
}

void uart0_putn(char *s, unsigned int n) {
  int i = 0;
  for (; i<n; i++) {
    //uart0_addr(&s[i]);
    //uart0_putc(' ');

    uart0_putc(s[i]);
    //uart0_putc(' ');
    //uart0_putc('\r');
    //uart0_putc('\n');
  }
}


void uart0_addr(void *a)
{
	int i;
  uint32_t addr = (uint32_t)a;
	unsigned char *ca = (unsigned char *)&addr;
	unsigned char h,l;

	for (i=3;i>=0;--i) {
		h = ca[i]/16;
		l = ca[i]%16;
		uart0_putc(h<10 ? h+0x30 : h-10+0x41);
		uart0_putc(l<10 ? l+0x30 : l-10+0x41);
	}
}

void uart0_itoa(unsigned int a)
{
	int i;
	unsigned char h,l;
  unsigned char *ca = (unsigned char *)&a;

	for (i=3;i>=0;--i) {
		h = (ca[i])/16;
		l = (ca[i])%16;
		uart0_putc(h<10 ? h+0x30 : h-10+0x41);
		uart0_putc(l<10 ? l+0x30 : l-10+0x41);
	}
}
