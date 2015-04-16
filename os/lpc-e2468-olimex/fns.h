#define coherence()     /* nothing needed for uniprocessor */
#define procsave(p)     /* Save the mach part of the current */ 
                        /* process state, no need for one cpu */

#define KADDR(p)    ((void *)p)
#define PADDR(p)    ((ulong)p)

#define	waserror()	(up->nerrlab++, setlabel(&up->errlab[up->nerrlab-1]))
//#define getcallerpc(x)        __builtin_return_address(0)
ulong	getcallerpc(void*);
void    (*screenputs)(char*, int);
void	(*idle)(void);
void	uartspecial(int, int, char, Queue**, Queue**, int (*)(Queue*, int));

#include "../port/portfns.h"

