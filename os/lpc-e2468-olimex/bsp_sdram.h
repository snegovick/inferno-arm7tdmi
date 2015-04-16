#ifndef __BSP_SDRAM_H__
#define __BSP_SDRAM_H__

#define SDRAM_BASE_ADDR (volatile unsigned int*)0xa0100000

void sdram_test (void);

#endif/*__BSP_SDRAM_H__*/
