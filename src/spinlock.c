#include "kernel/features/spinlck.h"
#include "stdlib.h"
#include "stdio.h"

void 
acquire(struct spinlock *lk)
{
    while(lk->locked || __sync_lock_test_and_set(&lk->locked, 1))
        ;
}

void 
release(struct spinlock *lk)
{
    if (!lk->locked)
		printf("ERROR: RELEASE: NOT LOCKED\n");
        panic();
    __sync_lock_test_and_set(&lk->locked, 0);
}