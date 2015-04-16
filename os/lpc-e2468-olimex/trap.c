#include "trap.h"
#include <stdbool.h>

//enum {
//	MinGpioIRQbit = 11,
//	NumGpioIRQbits = MaxGPIObit-MinGpioIRQbit+1,
//	GpioIRQmask = ((1<<NumGpioIRQbits)-1)<<MinGpioIRQbit,
//};

static Handler irqvec[MaxIRQbit+1];
//static Handler gpiovec[NumGpioIRQbits];
static Lock veclock;

bool trp_get_irqvec(int n, Handler *h) {
  if (n>=MaxIRQbit) {
    return false;
  }
  h = &irqvec[n];
  return true;
}

void
intrenable(int v, void (*f)(Ureg*, void*), void* a, int tbdf, char *name)
{
	int x;
	Handler *ie;

	ilock(&veclock);
	switch(tbdf) {
	case BUSUNKNOWN:
	case BusCPU:
		if(v < 0 || v > MaxIRQbit)
			panic("intrenable: irq source %d out of range", v);
		ie = &irqvec[v];
		if(ie->r != nil)
			iprint("duplicate irq: %d (%s)\n", v, ie->name);
		ie->r = f;
		ie->a = a;
		strncpy(ie->name, name, KNAMELEN-1);
		ie->name[KNAMELEN-1] = 0;

		//x = splfhi();
		/* Enable the interrupt by setting the mask bit */
		//splx(x);
		break;
	default:
		panic("intrenable: unknown irq bus %d", tbdf);
	}
	iunlock(&veclock);
}

void
trap(Ureg* ureg)
{
  pref_printf("bsp:: trap\r\n");
}
