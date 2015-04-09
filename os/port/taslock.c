#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

static void
lockloop(Lock *l, ulong pc)
{
  pref_printf("\r\n********************\r\nlockloop\r\n********************\r\n");
	setpanic();
	print("lock loop 0x%lux key 0x%lux pc 0x%lux held by pc 0x%lux\n", l, l->key, pc, l->pc);
	panic("lockloop");
}

void
lock(Lock *l)
{
	int i;
	ulong pc;

	pc = getcallerpc(&l);
  pref_printf("lock: pc = %x\r\n", pc);
	if(up == 0) {
    pref_printf("lock: up\r\n");
		if (_tas(&l->key) != 0) {
      pref_printf("lock: tas1\r\n");
			for(i=0; ; i++) {
				if(_tas(&l->key) == 0) {
          pref_printf("lock: tas2\r\n");
					break;
        }
				if (i >= 1000000) {
          pref_printf("lock: i>1000000\r\n");
					lockloop(l, pc);
					break;
				}
			}
		}
		l->pc = pc;
    pref_printf("lock: ret\r\n");
		return;
	}

  pref_printf("lock: loop\r\n");

	for(i=0; ; i++) {
		if(_tas(&l->key) == 0)
			break;
		if (i >= 1000) {
			lockloop(l, pc);
			break;
		}
		if(conf.nmach == 1 && up->state == Running && islo()) {
			up->pc = pc;
			sched();
		}
	}
	l->pri = up->pri;
	up->pri = PriLock;
	l->pc = pc;
  pref_printf("lock: end\r\n");
}

void
ilock(Lock *l)
{
  uart0_puts("ilock\r\n");
	ulong x, pc;
	int i;

  uart0_puts("call getcallerpc\r\n");
	pc = getcallerpc(&l);
  uart0_puts("call splhi\r\n");
	x = splhi();
  uart0_puts("splhi done\r\n");
	for(;;) {
    uart0_puts("in for\r\n");
		if(_tas(&l->key) == 0) {
			l->sr = x;
			l->pc = pc;
			return;
		}
    uart0_puts("ilock1\r\n");
		if(conf.nmach < 2)
			panic("ilock: no way out: pc 0x%lux: lock 0x%lux held by pc 0x%lux", pc, l, l->pc);
    uart0_puts("ilock2\r\n");
		for(i=0; ; i++) {
			if(l->key == 0)
				break;
			clockcheck();
			if (i > 100000) {
				lockloop(l, pc);
				break;
			}
		}
	}
}

int
canlock(Lock *l)
{
	if(_tas(&l->key))
		return 0;
	if(up){
		l->pri = up->pri;
		up->pri = PriLock;
	}
	l->pc = getcallerpc(&l);
	return 1;
}

void
unlock(Lock *l)
{
  pref_printf("in unlock\r\n");
	int p;

  pref_printf("unlock: checking key\r\n");
	if(l->key == 0) {
    pref_printf("unlock: not locked: pc %i\n", getcallerpc(&l));
		print("unlock: not locked: pc %lux\n", getcallerpc(&l));
  }
	p = l->pri;
	l->pc = 0;
	l->key = 0;
  pref_printf("unlock: call coherence\r\n");
	coherence();
  pref_printf("unlock: coherence done\r\n");
	if(up){
    pref_printf("unlock: up\r\n");
		up->pri = p;
		if(up->state == Running && anyhigher()) {
      pref_printf("unlock: sched\r\n");
			sched();
    }
	}

}

void
iunlock(Lock *l)
{
	ulong sr;

	if(l->key == 0)
		print("iunlock: not locked: pc %lux\n", getcallerpc(&l));
	sr = l->sr;
	l->pc = 0;
	l->key = 0;
	coherence();
	splxpc(sr);
}
