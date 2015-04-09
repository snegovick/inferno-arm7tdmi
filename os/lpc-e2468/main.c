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
  return 0;
}

void userinit(void) {
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
  volatile uint32_t i = 100000UL;
  while (i>0) {
    i--;
  }
  uart0_puts("M addr: ");
  uart0_addr(m);
  uart0_puts("\r\n");

  extern char _etext;
  extern char _data;
  extern char _edata;
  char *src = &_etext;
  char *dst = &_data;

  uart0_puts("src, dst, data: ");
  uart0_addr(src);
  uart0_puts(" ");
  uart0_addr(dst);
  uart0_puts(" ");
  uart0_addr(&_edata);
  uart0_puts("\r\n");

  
  /* ROM has data at end of text; copy it.  */
  while (dst < &_edata)
    *dst++ = *src++;

  uart0_puts("M addr: ");
  uart0_addr(m);
  uart0_puts("\r\n");

  uart0_puts("Initializing\r\n");
  extern char * _bss_start;
  extern char * _bss_end;
  //char buf[32];
  //m_itoa((int)m, buf);
  //uart0_puts("\r\nptr mach: ");
  //uart0_puts(buf);

	memset(m, 0, sizeof(Mach));	/* clear the mach struct */
  uart0_puts("DONE\r\n");
	conf.nmach = 1;
  //pref_printw("Call archreset");
	//archreset();
  //pref_printf("[DONE]\r\n");
	/* dmareset(); */

  pref_printw("Call quotefmtinstall");
	quotefmtinstall();
  pref_printf("[DONE]\r\n");

  pref_printw("Call confinit");
	confinit();
  pref_printf("[DONE]\r\n");
  pref_printw("Call xinit");
	xinit();
  pref_printf("[DONE]\r\n");
	/* mmuinit(); */
	/* poolinit(); */
	/* poolsizeinit(); */
	/* trapinit(); */
	/* clockinit();  */
  uart0_puts("Init UART...");
	printinit();
  uart0_puts("DONE\r\n");
	/* screeninit(); */
	procinit();
	links();
	chandevreset();

	print("%ld MHz id %8.8lux\n", (m->cpuhz)/1000000, getcpuid());
	print("\nInferno %s\n", VERSION);
	print("Vita Nuova\n");
	print("conf %s (%lud) jit %d\n\n",conffile, kerndate, cflag);

	userinit();
	schedinit();
	return 1;
}

void
confinit(void)
{
	ulong base;

	conf.topofmem = 0xa0000000+16*MB;
	m->cpuhz = 57600000;

	base = 0xa0008000+0x100000+0x1000;
	conf.base0 = base;

	conf.base1 = 0;
	conf.npage1 = 0;

	conf.npage0 = (conf.topofmem - base)/BY2PG;

	conf.npage = conf.npage0 + conf.npage1;
	conf.ialloc = (((conf.npage*(main_pool_pcnt))/100)/2)*BY2PG;

	conf.nproc = 100 + ((conf.npage*BY2PG)/MB)*5;
	conf.nmach = 1;

}

/*
 *  All traps come here.  It might be slightly slower to have all traps call trap
 *  rather than directly vectoring the handler.
 *  However, this avoids a lot of code duplication and possible bugs.
 *  trap is called splfhi().
 */
void trap(Ureg* ureg) {
  for (;;);
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
  uart0_puts("bsp::splx\r\n");
  return;
}
int     spllo(void) {
  uart0_puts("bsp::spllo\r\n");
  return 0;
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
    : [r0] "=r"(ret), [r1] "=r"(i)
  );

  return ret;
}

int     islo(void) {
  uart0_puts("bsp::islo\r\n");
  return 0;
}
int     setlabel(Label* l) {
  uart0_puts("bsp::setlabel\r\n");
  return 0;
}
void    gotolabel(Label* l) {
  uart0_puts("bsp::gotolabel\r\n");
  return;
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
ulong getcallerpc(void *x)
{
  ulong *u;
  int i;

  u = (ulong*)x;
  for (i=0; i<128; i++, u++)
    if (*u == (ulong)(u+3))
      return u[1];
  return ~0;
}

int     segflush(void* v, ulong u) {
  uart0_puts("bsp::segflush\r\n");
  return 0;
}
void
idlehands(void)
{
	//cpuidlecount++;
	//INTRREG->iccr = 1;	/* only unmasked interrupts will stop idle mode */
	//idle();
  uart0_puts("bsp::idlehands\r\n");
}

void    kprocchild(Proc *p, void (*func)(void*), void *arg) { return; }

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
    "swp %[t2], %[t1], [%[m]]\n"
    : [t1] "=&r" (x), [t2] "=&r" (ret)
    : [m] "r" (p)
    );
  return ret;
}

ulong   _tas(ulong* u) {
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
void    dumpstack(void) { return; }
void    reboot(void) { return; }
void    halt(void) { return; }

Timer*  addclock0link(void (*cb)(void), int i) { return 0; }
void    clockcheck(void) { return; }
int signof (int i) {return 1;}

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
