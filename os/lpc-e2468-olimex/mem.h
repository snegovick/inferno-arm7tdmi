#define KiB     1024u       /*! Kibi 0x0000000000000400 */
#define MiB     1048576u    /*! Mebi 0x0000000000100000 */
#define GiB     1073741824u /*! Gibi 000000000040000000 */

#define KZERO       0xa0008000              /*! kernel address space */
#define BY2PG       (4*KiB)                 /*! bytes per page */
#define BY2V        8                       /*! only used in xalloc.c */
#define MACHADDR    (KZERO+0x100000)          /*! Mach structure */
#define ROUND(s,sz) (((s)+(sz-1))&~(sz-1))
#define PGROUND(s)	ROUND(s, BY2PG)

#define KSTKSIZE    (8*KiB*2)
#define KSTACK      KSTKSIZE

/*
 * PSR
 */
#define PsrMusr		0x10 	/* mode */
#define PsrMfiq		0x11 
#define PsrMirq		0x12
#define PsrMsvc		0x13
#define PsrMabt		0x17
#define PsrMund		0x1B
#define PsrMsys		0x1F
#define PsrMask		0x1F

#define PsrDfiq		0x00000040	/* disable FIQ interrupts */
#define PsrDirq		0x00000080	/* disable IRQ interrupts */

#define PsrV		0x10000000	/* overflow */
#define PsrC		0x20000000	/* carry/borrow/extend */
#define PsrZ		0x40000000	/* zero */
#define PsrN		0x80000000	/* negative/less than */
