#include "u.h"
#include "../port/lib.h"
#include "../port/error.h"
#include "mem.h"
#include	"dat.h"
#include	"fns.h"
#include	<a.out.h>
#include	<dynld.h>

/*
 * null kernel interface to dynld, to stop libinterp moaning
 */

void*
dynimport(Dynobj* o_d, char* o_c, ulong o_u)
{
	return nil;
}

void
dynobjfree(Dynobj* o_d)
{
}

Dynobj*
kdynloadfd(int fd, Dynsym *tab, int ntab)
{
	USED(fd, tab, ntab);
	return nil;
}

int
kdynloadable(int o_i)
{
	return 0;
}

Dynobj*
dynld(int o_i)
{
	return nil;
}

int
dynldable(int o_i)
{
	return 0;
}
