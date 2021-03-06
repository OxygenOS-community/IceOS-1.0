#pragma once

static inline void
disb()
{
    asm volatile("dsb sy; isb");
}

/* Read Exception Syndrome Register (EL1) */
static inline uint64_t
resr()
{
    uint64_t r;
    asm volatile("mrs %[x], esr_el1" : [x]"=r"(r));
    return r;
}

/* Read Exception Link Register (EL1) */
static inline uint64_t
relr()
{
    uint64_t r;
    asm volatile("mrs %[x], elr_el1" : [x]"=r"(r));
    return r;
}

/* Load Exception Syndrome Register (EL1) */
static inline void
lesr()
{
    asm volatile("msr esr_el1, %[x]" : : [x]"r"(0));
}

/* Load vector base (virtual) address register (EL1) */
static inline void
lvbar(void *p)
{
    disb();
    asm volatile("msr vbar_el1, %[x]" : : [x]"r"(p));
    disb();
}

/* Load Translation Table Base Register 0 (EL1) */
static inline void
lttbr0(uint64_t p)
{
    asm volatile("msr ttbr0_el1, %[x]" : : [x]"r"(p));
    disb();
    asm volatile("tlbi vmalle1");
    disb();
}

/* Load Translation Table Base Register 1 (EL1) */
static inline void
lttbr1(uint64_t p)
{
    asm volatile("msr ttbr1_el1, %[x]" : : [x]"r"(p));
    disb();
    asm volatile("tlbi vmalle1");
    disb();
}

static inline int
cpuid()
{
    int64_t id;
    asm volatile("mrs %[x], mpidr_el1" : [x]"=r"(id));
    return id & 0xFF;
}