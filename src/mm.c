#include "kernel/features/memlayout.h"
#include "kernel/features/spinlck.h"
#include "kernel/drivers/mmu.h"
#include "kernel/features/mm.h"
#include "kernel/types.h"
#include "stdio.h"

struct freelist {
    void *next;
    void *start, *end;
} freelist;

static struct spinlock memlock;

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
static void *
freelist_alloc(struct freelist *f)
{
    void *p = f->next;
    if (p)
        f->next = *(void **)p;
    return p;
}

// Free the page of physical memory pointed at by v.
static void
freelist_free(struct freelist *f, void *v)
{
    *(void **)v = f->next;
    f->next = v;
}

void
free_range(void *start, void *end)
{
    acquire(&memlock);
    int cnt = 0;
    void *p = ROUNDUP((char *)start, PGSIZE);
    for (; p + PGSIZE <= end; p += PGSIZE, cnt ++) 
        freelist_free(&freelist, p);
    printf("free_range: 0x%x ~ 0x%x, %d pages\n");
    release(&memlock);
}

// Allocate sz size of physical memory.
// Returns 0 if failed else a pointer.
void *
kalloc(size_t sz)
{
    acquire(&memlock);
    //void *p = buddy_alloc(bsp, sz);
    void *p = freelist_alloc(&freelist);
    assert(p);

    release(&memlock);
    return p;
}

// Free the physical memory pointed at by v.
void
kfree(void *va)
{
    acquire(&memlock);

    freelist_free(&freelist, va);
    //buddy_free(bsp, va);
    release(&memlock);
}