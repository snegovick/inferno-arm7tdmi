#include "mem.h"

TEXT _start(SB), 1, $-4
  MOVW    $(MACHADDR+BY2PG-4),SP
	MOVW    $setR12(SB), R12	/* static base (SB) */
	BL      ,main(SB)
