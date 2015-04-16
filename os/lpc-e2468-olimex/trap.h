#ifndef __TRAP_H__
#define __TRAP_H__

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

#include "ureg.h"

#define MaxIRQbit 31

#define Timerbit(x) ((x)<=1 ? 4+(x) : 26+(x))

enum {
	BusCPU= 1,
	BusGPIOfalling= 2,	/* falling edge */
	BusGPIOrising = 3,	/* rising edge */
	BusGPIOboth = 4,	/* both edges */
	BusMAX= 4,
	BUSUNKNOWN= -1,
};

typedef struct Handler Handler;

struct Handler {
	void	(*r)(Ureg*, void*);
	void	*a;
	int	v;
	char	name[KNAMELEN];
	Handler	*next;
};


#endif/*__TRAP_H__*/
