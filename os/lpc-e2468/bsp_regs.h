#ifndef __BSP_REGS_H__
#define __BSP_REGS_H__

typedef unsigned long int uint32_t;
typedef unsigned short uint16_t;

#define BSP_CLKSRCSEL (*(volatile uint32_t *)0xE01FC10C)
#define BSP_PLLCON (*(volatile uint32_t *)0xE01FC080)
#define BSP_PLLCON_PLLE (1)
#define BSP_PLLCON_PLLC (1<<1)
#define BSP_PLLCFG (*(volatile uint32_t *)0xE01FC084)
#define BSP_PLLCFG_MSEL(v) (v & 0x7fff)
#define BSP_PLLCFG_NSEL(v) ((v << 16) & 0x00ff0000)
#define BSP_PLLSTAT (*(volatile uint32_t *)0xE01FC088)
#define BSP_PLLSTAT_PLOCK (1<<26)
#define BSP_PLLSTAT_PLLE (1<<24)
#define BSP_PLLSTAT_PLLC (1<<25)
#define BSP_PLLFEED (*(volatile uint32_t *)0xE01FC08C)
#define BSP_CCLKCFG (*(volatile uint32_t *)0xE01FC104)
#define BSP_USBCLKCFG (*(volatile uint32_t *)0xE01FC108)
#define BSP_IRCTRIM (*(volatile uint32_t *)0xE01FC1A4)
#define BSP_PCLKSEL0 (*(volatile uint32_t *)0xE01FC1A8)
#define BSP_PCLKSEL1 (*(volatile uint32_t *)0xE01FC1AC)

#define BSP_PCONP (*(volatile uint32_t *)0xE01FC0C4)
#define BSP_PCONP_UART0 (1<<3)

#define BSP_PCLK_SEL0 (*(volatile uint32_t *)0xE01FC1A8)
#define BSP_PCLK_SEL0_UART0_11 (1<<6 | 1<<7)

#define BSP_U0LCR (*(volatile uint32_t *)0xE000C00C)
#define BSP_U0LCR_DLAB (1<<7)
#define BSP_U0DLL (*(volatile uint32_t *)0xE000C000)
#define BSP_U0DLM (*(volatile uint32_t *)0xE000C004)
#define BSP_U0FCR (*(volatile uint32_t *)0xE000C008)
#define BSP_U0FCR_FIFO (1)
#define BSP_PINSEL0 (*(volatile uint32_t *)0xE002C000)
#define BSP_PINSEL0_UART0_P02 (1<<4)
#define BSP_PINSEL0_UART0_P03 (1<<6)
#define BSP_PINMODE0 (*(volatile uint32_t *)0xE002C040)
#define BSP_U0FDR (*(volatile uint32_t *)0xE000C028)
#define BSP_U0FDR_DIVADDVAL(v) ((v << 0) & 0x0F)
#define BSP_U0FDR_MULVAL(v) ((v<<4) & 0xF0)
#define BSP_U0THR (*(volatile uint32_t *)0xE000C000)
#define BSP_U0LSR (*(volatile uint32_t *)0xE000C014)
#define BSP_U0LSR_THRE (1<<5)

#define BSP_SCS (*(volatile uint32_t *)0xE01FC1A0)
#define BSP_SCS_OSCRANGE (1<<4)
#define BSP_SCS_OSCEN (1<<5)
#define BSP_SCS_OSCSTAT (1<<6)

#endif/*__BSP_REGS_H__*/
