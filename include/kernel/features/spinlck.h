#pragma once

struct spinlock {
    volatile int locked;
};
void acquire(struct spinlock *);
void release(struct spinlock *);