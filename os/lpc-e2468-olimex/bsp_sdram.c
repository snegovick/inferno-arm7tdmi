#include "bsp_sdram.h"
#include "bsp_uart.h"

void sdram_test (void)
{
  unsigned int i;

  // 32 bits access
  char s0[] = "32bit write start\r\n";
  uart0_puts(s0);
  for (i = 0; i < 0x200000-0x100000; i+=sizeof(unsigned int))
  {
    uart0_itoa(i);
    uart0_putc(':');
    uart0_addr(SDRAM_BASE_ADDR+i);
    uart0_putc('\r');
    uart0_putc('\n');
    *(SDRAM_BASE_ADDR+i) = i;
  }
  char s1[] = "32bit write done\r\n";
  uart0_puts(s1);  

  for (i = 0; i < 0x200000-0x100000; i+=sizeof(unsigned int ))
  {
    uart0_itoa(i);
    uart0_putc(':');
    uart0_addr(SDRAM_BASE_ADDR+i);
    uart0_putc('\r');
    uart0_putc('\n');
    if (*(SDRAM_BASE_ADDR+i) != i)
    {
      return;
    }
  }
  char s2[] = "32bit read done\r\n";
  uart0_puts(s2);


  // 16 bits access
  char s3[] = "16bit write start\r\n";
  uart0_puts(s3);
  for (i = 0; i < 0x10000; i+=sizeof(unsigned short))
  {
    uart0_putc('.');
    *(unsigned short*)(SDRAM_BASE_ADDR+i) = i;
  }
  char s4[] = "16bit write done\r\n";
  uart0_puts(s4);

  for (i = 0; i < 0x10000; i+=sizeof(unsigned short))
  {
    uart0_putc('.');
    if (*(unsigned short*)(SDRAM_BASE_ADDR+i) != i)
    {
      return;
    }
  }
  char s5[] = "16bit read done\r\n";
  uart0_puts(s5);


  // 8 bits access
  char s6[] = "8bit write start\r\n";
  uart0_puts(s6);
  for ( i = 0; i < 0x100; i+=sizeof(unsigned char))
  {
    uart0_putc('.');
    *(unsigned char*)(SDRAM_BASE_ADDR+i) = i;
  }
  char s7[] = "8bit write done\r\n";
  uart0_puts(s7);

  for (i = 0; i < 0x100; i+=sizeof(unsigned char))
  {
    uart0_putc('.');
    if (*(unsigned char*)(SDRAM_BASE_ADDR+i) != i)
    {
      return;
    }
  }
  char s8[] = "8bit read done\r\n";
  uart0_puts(s8);

  return;
}	
