#pragma once

#include <kernel/features/ssp.h>

#define SCTLR_RESERVED					(3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN  (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN (0 << 24)
#define SCLTR_I_CACHE_DISABLED  (0 << 12)
#define SCLTR_D_CACHE_DISABLED  (0 << 2)
#define SCLTR_MMU_DISABLED      (0 << 0)
#define SCLTR_MMU_ENABLED       (1 << 0)

#define SCLTR_VALUE_MMU_DISABLED (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCLTR_I_CACHE_DISABLED | SCLTR_D_CACHE_DISABLED | SCLTR_MMU_DISABLED)

#define HCR_RW    (1 << 31)
#define HCR_VALUE HCR_RW

#define SCR_RESERVED  (3 << 4)
#define SCR_RW        (1 << 10)
#define SCR_NS        (1 << 0)
#define SCR_VALUE     (SCR_RESERVED | SCR_RW | SCR_NS)

#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h     (5 << 0)
#define SPSR_EL2      (9 << 0)
#define SPSR_VALUE    (SPSR_MASK_ALL | SPSR_EL1h)

#define ESR_ELx_EC_SHIFT		26
#define ESR_ELx_EC_SVC64		0x15
#define ESR_ELx_EC_DABT_LOW		0x24