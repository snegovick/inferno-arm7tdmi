#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"
#include "version.h"

#include "LPC24xx.h"

#include "bsp_regs.h"
#include "bsp_uart.h"
#include <stdbool.h>

#include "../port/uart.h"
PhysUart* physuart[1];


void clk_init(void) {
  if (BSP_PLLSTAT & BSP_PLLSTAT_PLLC) {
    BSP_PLLCON = BSP_PLLCON_PLLE;
    BSP_PLLFEED = 0xAA;
    BSP_PLLFEED = 0x55;
  }
  BSP_PLLCON = 0;
  BSP_PLLFEED = 0xAA;
  BSP_PLLFEED = 0x55;

  BSP_SCS |= BSP_SCS_OSCEN;
  while (! (BSP_SCS & BSP_SCS_OSCSTAT)) {
    
  }
  BSP_CLKSRCSEL = 0x01;

  BSP_PLLCFG |= (BSP_PLLCFG_MSEL(11) | BSP_PLLCFG_NSEL(0));
  BSP_PLLFEED = 0xAA;
  BSP_PLLFEED = 0x55;

  BSP_PLLCON = BSP_PLLCON_PLLE;

  BSP_PLLFEED = 0xAA;
  BSP_PLLFEED = 0x55;

  BSP_CCLKCFG = 4;

  while (! (BSP_PLLSTAT & BSP_PLLSTAT_PLOCK)) {
  }
  BSP_PLLCON = (BSP_PLLCON_PLLE | BSP_PLLCON_PLLC);
  BSP_PLLFEED = 0xAA;
  BSP_PLLFEED = 0x55;
}


Mach *m = (Mach*)MACHADDR;
Proc *up = 0;
Vectorpage	*page0 = (Vectorpage*)KZERO;	/* doubly-mapped to AIVECADDR */
Conf conf;

extern ulong kerndate;
extern int cflag;
extern int main_pool_pcnt;
extern int heap_pool_pcnt;
extern int image_pool_pcnt;
ulong cpuidlecount;

uint32_t getcpuid(void) {
  bsp_printf("bsp:getcpuid\r\n");
  return 0xabcdef;
}

void
init0(void)
{
  pref_printf("in init0\r\n");
	Osenv *o;
	char buf[2*KNAMELEN];
//	char *buf;

  pref_printf("init0 up\r\n");

	up->nerrlab = 0;

  pref_printf("init0 calling spllo\r\n");
	spllo();
  pref_printf("init0 done\r\n");

	if(waserror())
		panic("init0 %r");

  pref_printf("init0 no error\r\n");
	/*
	 * These are o.k. because rootinit is null.
	 * Then early kproc's will have a root and dot.
	 */
	o = up->env;
  pref_printf("                         init0 -1\r\n");
	o->pgrp->slash = namec("#/", Atodir, 0, 0);
  pref_printf("                         init0 0\r\n");
	cnameclose(o->pgrp->slash->name);
  pref_printf("                         init0 1\r\n");
	o->pgrp->slash->name = newcname("/");
  pref_printf("init0:: slashname->len: %i, slash addr: %x, slashname addr: %x\r\n", o->pgrp->slash->name->len, o->pgrp->slash, o->pgrp->slash->name);
	o->pgrp->dot = cclone(o->pgrp->slash);
  pref_printf("init0:: slashname->len: %i, slash addr: %x, slashname addr: %x\r\n", o->pgrp->slash->name->len, o->pgrp->slash, o->pgrp->slash->name);

	chandevinit();

  pref_printf("                         init0 after chandevinit\r\n");

	/* if(!waserror()){ */
	/* 	ksetenv("cputype", "arm", 0); */
	/* 	snprint(buf, sizeof(buf), "arm %s", conffile); */
	/* 	ksetenv("terminal", buf, 0); */
	/* 	poperror(); */
	/* } */

	/* poperror(); */

  pref_printf("call osinit.dis\r\n");
	disinit("/osinit.dis");
}

void userinit(void) {
	Proc *p;
	Osenv *o;

	p = newproc();
	o = p->env;

	o->fgrp = newfgrp(nil);
	o->pgrp = newpgrp();
	o->egrp = newegrp();
	kstrdup(&o->user, eve);

	strcpy(p->text, "interp");
	p->fpstate = FPINIT;

	/*
	 * Kernel Stack
	 *
	 * N.B. The -12 for the stack pointer is important.
	 *	4 bytes for gotolabel's return PC
	 */
	p->sched.pc = (ulong)init0;
	p->sched.sp = (ulong)p->kstack+KSTACK-8;

  pref_printf("userinit sched.pc: %x\r\n", p->sched.pc);

  pref_printf("userinit ready\r\n");
	ready(p);
}

static void
poolsizeinit(void)
{
	ulong nb;

	nb = conf.npage*BY2PG;
	poolsize(mainmem, (nb*main_pool_pcnt)/100, 0);
	poolsize(heapmem, (nb*heap_pool_pcnt)/100, 0);
	poolsize(imagmem, (nb*image_pool_pcnt)/100, 1);
}

void
archconsole(void)
{
	uartspecial(0, 115200, 'n', &kbdq, &printq, kbdcr2nl);
}

int main(int argc, char** argv) {
  //clk_init();
  //uart0_init();
  PINSEL9 &= ~(1 | 1<<1 | 1<<2 | 1<<3);
  PINMODE9 &= ~(1 | 1<<1 | 1<<2 | 1<<3);
  FIO4MASK &= ~(1<<17 | 1<<16);
  FIO4DIR |= (1<<17 | 1<<16);
  FIO4CLR |= 1<<17;
  FIO4SET |= 1<<16;
  
	char *s = "Inferno compiled with arm-none-eabi toolchain\r\n";
  uart0_puts(s);

  uart0_puts("Initializing\r\n");

	memset(m, 0, sizeof(Mach));	/* clear the mach struct */
  uart0_puts("DONE\r\n");
	conf.nmach = 1;

  bsp_printw("Call quotefmtinstall");
	quotefmtinstall();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call confinit");
	confinit();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call xinit");
	xinit();
  bsp_printf("[DONE]\r\n");
	/* mmuinit(); */

  bsp_printw("Call poolinit");
	poolinit();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call poolsizeinit");
	poolsizeinit();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call clockinit");
	clockinit();
  bsp_printf("[DONE]\r\n");

  uart0_puts("Init UART...");
	printinit();
  uart0_puts("DONE\r\n");

  //bsp_printw("Call trap init");
	//trapinit();
  //bsp_printf("[DONE]\r\n");
  
	/* screeninit(); */
  bsp_printw("Call procinit");
	procinit();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call links");
	links();
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call chandevreset");
	chandevreset();
  bsp_printf("[DONE]\r\n");

  eve = strdup("inferno");
  archconsole();

  bsp_printw("Calling PRINT");
	print("%ld MHz id %8.8lux\n", (m->cpuhz)/1000000, getcpuid());
	print("\nInferno %s\n", VERSION);
	print("Vita Nuova\n");
	print("conf %s (%lud) jit %d\n\n",conffile, kerndate, cflag);
  bsp_printf("[DONE]\r\n");

  bsp_printw("Call userinit");
	userinit();
  bsp_printf("[DONE]\r\n");

  pref_printw("Call sched init\r\n");
	schedinit();
  pref_printf("[DONE]\r\n");
  pref_printf("\r\nRETURN 1\r\n");
	return 1;
}

void
confinit(void)
{
	ulong base;

	conf.topofmem = 0xa0000000+8*MB;
	m->cpuhz = 57600000;

	base = 0xa0008000+0x100000+0x1000;
	conf.base0 = base;

	conf.base1 = 0;
	conf.npage1 = 0;

	conf.npage0 = (conf.topofmem - base)/BY2PG;

	conf.npage = conf.npage0 + conf.npage1;
	conf.ialloc = (((conf.npage*(main_pool_pcnt))/100)/2)*BY2PG;

//	conf.nproc = 100 + ((conf.npage*BY2PG)/MB)*5;
	conf.nproc = 20;
	conf.nmach = 1;

}

ulong va2pa(void *v)
{
  return (ulong)v;
}

/* int waserror(void) */
/* { */
/*   return 0; */
/* } */
int splhi(void) { 
  /* TEXT splhi(SB), $-4					 */
	/* MOVW		CPSR, R0 */
	/* ORR		$(PsrDirq), R0, R1 */
	/* MOVW		R1, CPSR */
	/* MOVW	$(MACHADDR), R6 */
	/* MOVW	R14, (R6)	/\* m->splpc *\/ */
	/* RET */
  register uint32_t ret, tmp;

  __asm__ __volatile__
  (
    "mrs %[r0], CPSR" "\n\t"
    "orr %[r1], %[r0], #0x80" "\n\t"
    "msr CPSR_c, %[r1]" "\n\t"
    "mov %[dst], lr\n"
    : [dst] "=&r" (m->splpc), [r0] "=r"(ret), [r1] "=r"(tmp)
  );

  return ret;
}

void    splx(int i)
{
  __asm__ __volatile__ (
    "mov %[dst], lr\n"
    : [dst] "=&r" (m->splpc)
    );
}
int     spllo(void) {
  register uint32_t ret, tmp;
  uint32_t val = PsrDirq | PsrDfiq;

  __asm__ __volatile__
  (
    "mrs %[r0], CPSR\n\t"
    "bic %[r1], %[r0], %[val]\n\t"
    "msr CPSR_c, %[r1]\n\t"
    : [r0] "=r"(ret), [r1] "=r"(tmp)
    : [val] "r" (val)
  );

  return ret;
}
void splxpc(int i) {
	/* MOVW		R0, R1 */
	/* MOVW		CPSR, R0 */
	/* MOVW		R1, CPSR */
	/* RET */
  register uint32_t ret;

  __asm__ __volatile__
  (
    "mrs %[r0], CPSR" "\n\t"
    "msr CPSR_c, %[r1]" "\n\t"
    : [r0] "=r"(ret)
    : [r1] "r"(i)
  );

  return;
}

int     islo(void) {
  register uint32_t ret;
  uint32_t val = PsrDirq;

  __asm__ __volatile__
  (
    "mrs %[r0], CPSR\n\t"
    "and %[r0], %[val]\n\t"
    "eor %[r0], %[val]\n\t"
    : [r0] "=r"(ret)
    : [val] "r" (val)
  );

  return ret;
}

//int    setlabel(Label* l) __attribute__((naked));
int    setlabel(Label* l) {
  __asm__ __volatile__ (
    "mov %[dstsp], sp\n"
    "mov %[dstpc], lr\n"
    : [dstsp] "=&r" (l->sp), [dstpc] "=&r" (l->pc)
    );

  return 0;
}

//void    gotolabel(Label* l) __attribute__((naked));
void    gotolabel(Label* l) {
  __asm__ __volatile__ (
    "mov sp, %[srcsp]\n"
    "mov lr, %[srcpc]\n"
    "bx lr"
    :
    : [srcsp] "r" (l->sp), [srcpc] "r" (l->pc)
    );
//  void (*f)(void) = l->pc;
//  f();
  //return;
}
/*
* Functions generated by arm-elf-gcc start
* like this (always?):
* mov ip, sp
* stmdb sp!, {fp, ip, lr, pc}
*
* (The first argument, which is still in r0,
* is pushed later, when its address is taken so
* that it can be provided to getcallerpc.)
*
* To get the value of `lr', the return address,
* walk up the stack, starting with x, until a
* value *u is found which is the previous stack
* pointer, i.e. which equals the address u+3.
* In that case, the return address is expected
* to be in u[1].
*
* If no such address is found, return an invalid value.
*/
/* ulong getcallerpc(void *x) __attribute__((naked)); */
/* ulong getcallerpc(void *x) */
/* { */
/*   /\* ulong *u; *\/ */
/*   /\* int i; *\/ */

/*   /\* u = (ulong*)x; *\/ */
/*   /\* for (i=0; i<128; i++, u++) *\/ */
/*   /\*   if (*u == (ulong)(u+3)) *\/ */
/*   /\*     return u[1]; *\/ */
/*   /\* return ~0; *\/ */
/*   ulong ret; */
/*   __asm__ __volatile__ ( */
/*     "mov %[ret], %%lr\n" */
/*     : [ret] "=r" (ret) */
/*     ); */
/*   return ret; */
/* } */

int     segflush(void* v, ulong u) {
  return 1;
}
void
idlehands(void)
{
  uart0_puts("bsp::idlehands\r\n");
	cpuidlecount++;
	//INTRREG->iccr = 1;	/* only unmasked interrupts will stop idle mode */
	idle();
}

static void
linkproc(void)
{
	spllo();
	if (waserror())
		print("error() underflow: %r\n");
	else
		(*up->kpfun)(up->arg);
	pexit("end proc", 1);
}

void
kprocchild(Proc *p, void (*func)(void*), void *arg)
{
	p->sched.pc = (ulong)linkproc;
	p->sched.sp = (ulong)p->kstack+KSTACK-8;

	p->kpfun = func;
	p->arg = arg;
}

/* inline uint32_t atomic_increment(uint32_t * memory) { */
/*   uint32_t temp1, temp2; */
/*   __asm__ __volatile__ ( */
/*     "1:\n" */
/*     "    ldrex  %[t1],[%[m]]\n" */
/*     "    add    %[t1],%[t1],#1\n" */
/*     "    strex  %[t2],%[t1],[%[m]]\n" */
/*     "    cmp    %[t2],#0\n" */
/*     "    bne\t1b" */
/*     : [t1] "=&r" (temp1), [t2] "=&r" (temp2) */
/*     : [m] "r" (memory) */
/*   ); */
/*   return temp1; */
/* } */

/* uint32_t __ldrex(volatile uint32_t *p) { */
/*   uint32_t ret; */
/*   __asm__ __volatile__ ( */
/*     "ldrex %[t1], [%[m]]\n" */
/*     : [t1] "=&r" (ret) */
/*     : [m] "r" (p) */
/*     ); */
/*   return ret; */
/* } */

/* uint32_t __strex(uint32_t x, volatile uint32_t *p) { */
/*   uint32_t ret; */
/*   __asm__ __volatile__ ( */
/*     "strex %[t1], %[t2], [%[m]]\n" */
/*     : [t1] "=&r" (ret), [t2] "=&r" (x) */
/*     : [m] "r" (p) */
/*     ); */
/*   return ret; */
/* } */

/* uint32_t __swp(uint32_t x, volatile uint32_t *p) { */
/*   uint32_t v; */
/* /\* use */
/*    LDREX/STREX */
/*    intrinsics not specified by ACLE *\/ */
/*   do v = __ldrex(p); while (__strex(x, p)); */
/*   return v; */
/* } */

uint32_t __swp(uint32_t x, volatile uint32_t *p) {
  uint32_t ret;
  __asm__ __volatile__ (
    "swp %[ret], %[x], [%[m]]\n"
    : [ret] "=&r" (ret)
    : [m] "r" (p), [x] "r" (x)
    );
  return ret;
}

ulong   _tas(ulong* u) {
  /* uint32_t ret = *u; */
  /* *u = 0xdeaddead; */
  /* pref_printf("tas:: u: %x, ret: %x\r\n", *u, ret); */
  /* return ret; */
  return __swp(0xDEADDEAD, u);
}

ulong   _div(ulong* u) {
  uart0_puts("bsp::_div\r\n");
  return 0;
}
ulong   _divu(ulong* u) {
  uart0_puts("bsp::_divu\r\n");
  return 0;
}
ulong   _mod(ulong* u) {
  uart0_puts("bsp::_mod\r\n");
  return 0;
}
ulong   _modu(ulong* u) {
  uart0_puts("bsp::_modu\r\n");
  return 0;
}

void    setpanic(void) {
  pref_printf("Infernal panic\r\n");
  while (1);
}
void    reboot(void) {
  bsp_printf("bsp:reboot\r\n");
  return;
}
void    halt(void) {
  bsp_printf("bsp:halt\r\n");
  return;
}

int signof (int i) {
  bsp_printf("bsp:signof\r\n");
  return 1;
}

/* void    fpinit(void) {} */
/* void    FPsave(void*) {} */
/* void    FPrestore(void*) {} */


/* stubs */
void
setfsr(ulong u)
{
}

ulong
getfsr()
{
	return 0;
}

void
setfcr(ulong u)
{
}

ulong
getfcr()
{
	return 0;
}

void
fpinit(void)
{
}

void
FPsave(void* v)
{
}

void
FPrestore(void* v)
{
}

void exit(int status) {};
