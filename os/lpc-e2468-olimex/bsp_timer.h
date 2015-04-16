#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "bsp_regs.h"

#define CLOCKFREQ 57600000

void timer_enable(unsigned int timer, unsigned int freq);
void timer_disable(unsigned int timer);
uint32_t timer_get_tc(unsigned int timer);

#endif/*__BSP_TIMER_H__*/
