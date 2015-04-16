#include "bsp_regs.h"
#include "bsp_timer.h"
#include "lpc24xx_irq.h"

#include "trap.h"

static void timer0_irq_wrapper(void) {
  Handler h;
  if (trp_get_irqvec(Timerbit(0)))
    h.r(0, h.a);
  BSP_T0IR |= BSP_TXIR_MR0;
}

void timer_enable(unsigned int timer, unsigned int freq) {
  uint32_t mr = CLOCKFREQ/(freq-1);
  switch (timer) {
  case 0:
    BSP_PCONP |= BSP_PCONP_TIM0;
    BSP_PCLK_SEL0_SET_TIM0_01;
    BSP_T0TCR &= ~BSP_TXTCR_CR;
    //BSP_T0MCR |= BSP_TXMCR_MR0I | BSP_TXMCR_MR0S;
    BSP_T0MCR |= BSP_TXMCR_MR0I;
    BSP_T0MR0 = mr;
    BSP_T0IR |= BSP_TXIR_MR0;
    BSP_T0PR = 0;
    install_irq( TIMER0_INT, (void *)timer0_irq_wrapper, 1 );
    BSP_T0TCR = BSP_TXTCR_CE;
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  default:
    return;
  }
  return;
}

void timer_disable(unsigned int timer) {
  BSP_T0TCR &= ~BSP_TXTCR_CE;
  BSP_T0TCR |= BSP_TXTCR_CR;
}

uint32_t timer_get_tc(unsigned int timer) {
  uint32_t t;
  switch (timer) {
  case 0:
    t = BSP_T0TC;
    break;
  case 1:
    t = BSP_T1TC;
    break;
  case 2:
    t = BSP_T2TC;
    break;
  case 3:
    t = BSP_T3TC;
    break;
  default:
    return 0;
  }
  return t;
}
