#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"

#define datoff		((ulong)((Xhdr*)0)->data)

enum
{
	Chunk		= 64*1024,
	Nhole		= 128,
	Magichole	= 0xDeadBabe,
};

typedef struct Hole Hole;
typedef struct Xalloc Xalloc;
typedef struct Xhdr Xhdr;

struct Hole
{
	ulong	addr;
	ulong	size;
	ulong	top;
	Hole*	link;
};

struct Xhdr
{
	ulong	size;
	ulong	magix;
	char	data[1];
};

struct Xalloc
{
	Lock;
	Hole	hole[Nhole];
	Hole*	flist;
	Hole*	table;
};

static Xalloc	xlists;

static void
ixprt()
{
	xsummary();
	ixsummary();
}

void
xinit(void)
{
	Hole *h, *eh;

	eh = &xlists.hole[Nhole-1];
	for(h = xlists.hole; h < eh; h++)
		h->link = h+1;

	xlists.flist = xlists.hole;

	if(conf.npage1)
		xhole(conf.base1, conf.npage1*BY2PG);
	conf.npage1 = conf.base1+(conf.npage1*BY2PG);

	if(conf.npage0)
		xhole(conf.base0, conf.npage0*BY2PG);
	conf.npage0 = conf.base0+(conf.npage0*BY2PG);

	/* Save the bounds of kernel alloc memory for kernel mmu mapping */
	conf.base0 = (ulong)KADDR(conf.base0);
	conf.base1 = (ulong)KADDR(conf.base1);
	conf.npage0 = (ulong)KADDR(conf.npage0);
	conf.npage1 = (ulong)KADDR(conf.npage1);

	debugkey('x', "xalloc/ialloc", ixprt, 0);
}

void*
xspanalloc(ulong size, int align, ulong span)
{
	ulong a, v, t;

	a = (ulong)xalloc(size+align+span);
	if(a == 0)
		panic("xspanalloc: %lud %d %lux\n", size, align, span);

	if(span > 2) {
		v = (a + span) & ~(span-1);
		t = v - a;
		if(t > 0)
			xhole(PADDR(a), t);
		t = a + span - v;
		if(t > 0)
			xhole(PADDR(v+size+align), t);
	}
	else
		v = a;

	if(align > 1)
		v = (v + align) & ~(align-1);

	return (void*)v;
}

void*
xallocz(ulong size, int zero)
{
  uart0_puts("in xalloc\r\n");
	Xhdr *p;
	Hole *h, **l;

	size += BY2V + sizeof(Xhdr);
	size &= ~(BY2V-1);

  uart0_puts("xalloc: call ilock\r\n");
	ilock(&xlists);
  uart0_puts("xalloc: ilock done\r\n");
  
	l = &xlists.table;
	for(h = *l; h; h = h->link) {
    uart0_puts("xalloc: in for\r\n");
		if(h->size >= size) {
			p = (Xhdr*)h->addr;
			h->addr += size;
			h->size -= size;
			if(h->size == 0) {
				*l = h->link;
				h->link = xlists.flist;
				xlists.flist = h;
			}
			iunlock(&xlists);
			p = KADDR(p);
			p->magix = Magichole;
			p->size = size;
			if(zero)
				memset(p->data, 0, size);
			return p->data;
		}
		l = &h->link;
	}
	iunlock(&xlists);
	return nil;
}

void*
xalloc(ulong size)
{
	return xallocz(size, 1);
}

void
xfree(void *p)
{
	Xhdr *x;

	x = (Xhdr*)((ulong)p - datoff);
	if(x->magix != Magichole) {
		xsummary();
		panic("xfree(0x%lux) 0x%lux!=0x%lux", p, (ulong)Magichole, x->magix);
	}
	xhole(PADDR(x), x->size);
}

int
xmerge(void *vp, void *vq)
{
	Xhdr *p, *q;

	p = (Xhdr*)(((ulong)vp - offsetof(Xhdr, data[0])));
	q = (Xhdr*)(((ulong)vq - offsetof(Xhdr, data[0])));
	if(p->magix != Magichole || q->magix != Magichole) {
		xsummary();
		panic("xmerge(%#p, %#p) bad magic %#lux, %#lux\n",
			vp, vq, p->magix, q->magix);
	}
	if((uchar*)p+p->size == (uchar*)q) {
		p->size += q->size;
		return 1;
	}
	return 0;
}

void
xhole(ulong addr, ulong size)
{
	ulong top;
	Hole *h, *c, **l;

	if(size == 0)
		return;

	top = addr + size;
	ilock(&xlists);
	l = &xlists.table;
	for(h = *l; h; h = h->link) {
		if(h->top == addr) {
			h->size += size;
			h->top = h->addr+h->size;
			c = h->link;
			if(c && h->top == c->addr) {
				h->top += c->size;
				h->size += c->size;
				h->link = c->link;
				c->link = xlists.flist;
				xlists.flist = c;
			}
			iunlock(&xlists);
			return;
		}
		if(h->addr > addr)
			break;
		l = &h->link;
	}
	if(h && top == h->addr) {
		h->addr -= size;
		h->size += size;
		iunlock(&xlists);
		return;
	}

	if(xlists.flist == nil) {
		iunlock(&xlists);
		print("xfree: no free holes, leaked %lud bytes\n", size);
		return;
	}

	h = xlists.flist;
	xlists.flist = h->link;
	h->addr = addr;
	h->top = top;
	h->size = size;
	h->link = *l;
	*l = h;
	iunlock(&xlists);
}

void
xsummary(void)
{
	int i;
	Hole *h;

	i = 0;
	for(h = xlists.flist; h; h = h->link)
		i++;

	print("%d holes free\n", i);
	i = 0;
	for(h = xlists.table; h; h = h->link) {
		print("%.8lux %.8lux %lud\n", h->addr, h->top, h->size);
		i += h->size;
	}
	print("%d bytes free\n", i);
}
