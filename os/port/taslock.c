#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

static void
lockloop(Lock *l, ulong pc)
{
  dis_printf("\r\n********************\r\nlockloop\r\n********************\r\n");
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
  dis_printf("lock: pc = %x, key: %x, lockptr: %x\r\n", pc, (ulong)(l->key), (ulong)l);
	if(up == 0) {
    bsp_printf("lock: up\r\n");
		if (_tas(&l->key) != 0) {
      bsp_printf("lock: tas1\r\n");
			for(i=0; ; i++) {
				if(_tas(&l->key) == 0) {
          bsp_printf("lock: tas2\r\n");
					break;
        }
				if (i >= 1000000) {
          bsp_printf("lock: i>1000000\r\n");
					lockloop(l, pc);
					break;
				}
			}
		}
		l->pc = pc;
    bsp_printf("lock: ret\r\n");
		return;
	}

  bsp_printf("lock: loop\r\n");

	for(i=0; ; i++) {
    unsigned int tasret = _tas(&l->key);
    bsp_printf("lock: %i, up->state: %i, islo: %i, tasret: %x\r\n", i, up->state, islo(), tasret);
//		if(_tas(&l->key) == 0)
    if(tasret == 0)
			break;
		if (i >= 100) {
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
  bsp_printf("lock: end\r\n");
}

void
ilock(Lock *l)
{
  bsp_printf("ilock\r\n");
	ulong x, pc;
	int i;

  bsp_printf("call getcallerpc\r\n");
	pc = getcallerpc(&l);
  bsp_printf("call splhi\r\n");
	x = splhi();
  bsp_printf("splhi done\r\n");
	for(;;) {
    bsp_printf("in for\r\n");
		if(_tas(&l->key) == 0) {
			l->sr = x;
			l->pc = pc;
			return;
		}
    bsp_printf("ilock1\r\n");
		if(conf.nmach < 2)
			panic("ilock: no way out: pc 0x%lux: lock 0x%lux held by pc 0x%lux", pc, l, l->pc);
    bsp_printf("ilock2\r\n");
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
  dis_printf("in unlock, key: %x, lockptr: %x\r\n", (ulong)(l->key), (ulong)l);
	int p;

  bsp_printf("unlock: checking key\r\n");
	if(l->key == 0) {
    //dis_printf("unlock: not locked: pc %x\n", getcallerpc(&l));
    bsp_printf("unlock: not locked: pc\r\n");// %x\n", getcallerpc(&l));
		//print("unlock: not locked: pc %lux\n", getcallerpc(&l));
  }
  bsp_printf("unlock: after key check\r\n");
	p = l->pri;
	l->pc = 0;
	l->key = 0;
  bsp_printf("unlock: call coherence\r\n");
	coherence();
  bsp_printf("unlock: coherence done\r\n");
	if(up){
    bsp_printf("unlock: up\r\n");
		up->pri = p;
		if(up->state == Running && anyhigher()) {
      bsp_printf("unlock: sched\r\n");
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
