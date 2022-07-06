/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include <nds32_intrinsic.h>
#include "nds32_defs.h"
#include "ae210p.h"
#include "cache.h"


#define CACHE_NONE              0
#define CACHE_WRITEBACK         2
#define CACHE_WRITETHROUGH      3

#ifdef CFG_CACHE_ENABLE
/* Cacheable */
#ifdef CFG_CACHE_WRITETHROUGH
#define CACHE_MODE              CACHE_WRITETHROUGH
#else
#define CACHE_MODE              CACHE_WRITEBACK
#endif
#else
/* Uncacheable */
#define CACHE_MODE              CACHE_NONE
#endif


#define MMU_CTL_MSK                                     \
        (MMU_CTL_mskD                                   \
         | MMU_CTL_mskNTC0                              \
         | MMU_CTL_mskNTC1                              \
         | MMU_CTL_mskNTC2                              \
         | MMU_CTL_mskNTC3                              \
         | MMU_CTL_mskTBALCK                            \
         | MMU_CTL_mskMPZIU                             \
         | MMU_CTL_mskNTM0                              \
         | MMU_CTL_mskNTM1                              \
         | MMU_CTL_mskNTM2                              \
         | MMU_CTL_mskNTM3)
/*
 * NTC0: CACHE_MODE, NTC1~NTC3: Non-cacheable
 * NTM0~NTM3 are mapped to partition 0/1/2/3
 */
#define MMU_CTL_INIT                                    \
        (0x0UL << MMU_CTL_offD                          \
         | (CACHE_MODE) << MMU_CTL_offNTC0              \
         | 0x0UL << MMU_CTL_offNTC1                     \
         | 0x0UL << MMU_CTL_offNTC2                     \
         | 0x0UL << MMU_CTL_offNTC3                     \
         | 0x0UL << MMU_CTL_offTBALCK                   \
         | 0x0UL << MMU_CTL_offMPZIU                    \
         | 0x0UL << MMU_CTL_offNTM0                     \
         | 0x1UL << MMU_CTL_offNTM1                     \
         | 0x2UL << MMU_CTL_offNTM2                     \
         | 0x3UL << MMU_CTL_offNTM3)

#define CACHE_CTL_MSK                                   \
        (CACHE_CTL_mskIC_EN                             \
         | CACHE_CTL_mskDC_EN                           \
         | CACHE_CTL_mskICALCK                          \
         | CACHE_CTL_mskDCALCK                          \
         | CACHE_CTL_mskDCCWF                           \
         | CACHE_CTL_mskDCPMW)

/* ICache/DCache enable */
#define CACHE_CTL_CACHE_ON                              \
        (0x1UL << CACHE_CTL_offIC_EN                    \
         | 0x1UL << CACHE_CTL_offDC_EN                  \
         | 0x0UL << CACHE_CTL_offICALCK                 \
         | 0x0UL << CACHE_CTL_offDCALCK                 \
         | 0x1UL << CACHE_CTL_offDCCWF                  \
         | 0x1UL << CACHE_CTL_offDCPMW)

/*
 * Interrupt priority
 * Default: lowest priority
 */
#define PRI1_DEFAULT            0xFFFFFFFF
#define PRI2_DEFAULT            0xFFFFFFFF


/* This must be a leaf function, no child function */
void _nds32_init_mem(void) __attribute__((naked));
void _nds32_init_mem(void)
{
	/* Enable DLM */
	__nds32__mtsr_dsb(EDLM_BASE | 0x1, NDS32_SR_DLMB);
}

/* Overwrite c_startup weak function */
void c_startup(void)
{
#define MEMCPY(des, src, n)     __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))
	/* Data section initialization */
	extern char __data_lmastart, __data_start, _edata;
	extern char __bss_start, _end;
	unsigned int size = &_edata - &__data_start;

	/* Copy data section from LMA to VMA */
	MEMCPY(&__data_start, &__data_lmastart, size);

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
}

/*
 * Initialize MMU configure and cache ability.
 */
static void mmu_init(void)
{
#ifndef __NDS32_ISA_V3M__
	unsigned int reg;

	/* MMU initialization: NTC0~NTC3, NTM0~NTM3 */
	reg = (__nds32__mfsr(NDS32_SR_MMU_CTL) & ~MMU_CTL_MSK) | MMU_CTL_INIT;
	__nds32__mtsr_dsb(reg, NDS32_SR_MMU_CTL);
#endif
}

/*
 * Platform specific initialization
 */
static void plf_init(void)
{
	/* Set default Hardware interrupts priority */
	__nds32__mtsr(PRI1_DEFAULT, NDS32_SR_INT_PRI);
	__nds32__mtsr(PRI2_DEFAULT, NDS32_SR_INT_PRI2);
}

/*
 * All AE210P system initialization
 */
void system_init(void)
{
	mmu_init();                     /* mmu/cache */
	plf_init();                     /* Perform any platform specific initializations */

#ifdef CFG_CACHE_ENABLE
	unsigned int reg;

	/* Invalid ICache */
	nds32_icache_flush();

	/* Invalid DCache */
	nds32_dcache_invalidate();

	/* Enable I/Dcache */
	reg = (__nds32__mfsr(NDS32_SR_CACHE_CTL) & ~CACHE_CTL_MSK) | CACHE_CTL_CACHE_ON;
	__nds32__mtsr(reg, NDS32_SR_CACHE_CTL);
#endif
}
