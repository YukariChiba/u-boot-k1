/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

#ifndef _ASM_RISCV_CACHE_H
#define _ASM_RISCV_CACHE_H

#ifndef __ASSEMBLY__
#define cbo_clean(start)			\
	({								\
		unsigned long __v = (unsigned long)(start); \
		__asm__ __volatile__("cbo.clean"	\
							 " 0(%0)"		\
							 :				\
							 : "rK"(__v)	\
							 : "memory");	\
	})

#define cbo_invalid(start)			\
	({								\
		unsigned long __v = (unsigned long)(start); \
		__asm__ __volatile__("cbo.inval"	\
							 " 0(%0)"		\
							 :				\
							 : "rK"(__v)	\
							 : "memory");	\
	})

#define cbo_flush(start)			\
	({								\
		unsigned long __v = (unsigned long)(start); \
		__asm__ __volatile__("cbo.flush"	\
							 " 0(%0)"		\
							 :				\
							 : "rK"(__v)	\
							 : "memory");	\
	})
#endif /* __ASSEMBLY__ */

/* cache */
void cache_flush(void);

/*
 * The current upper bound for RISCV L1 data cache line sizes is 32 bytes.
 * We use that value for aligning DMA buffers unless the board config has
 * specified an alternate cache line size.
 */
#ifdef CONFIG_SYS_CACHELINE_SIZE
#define ARCH_DMA_MINALIGN	CONFIG_SYS_CACHELINE_SIZE
#else
#define ARCH_DMA_MINALIGN	32
#endif

#endif /* _ASM_RISCV_CACHE_H */
