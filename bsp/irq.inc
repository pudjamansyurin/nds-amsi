#ifndef __IRQ_INC__
#define __IRQ_INC__

	/* FPU Caller registers */
	.macro SAVE_FPU_CALLER_REGS_00
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_CALLER_REGS_01
		SAVE_FPU_CALLER_REGS_00
	.endm

	.macro SAVE_FPU_CALLER_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_CALLER_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd23, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd21, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd19, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd17, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro push_fpu_caller
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		SAVE_FPU_CALLER_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		SAVE_FPU_CALLER_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		SAVE_FPU_CALLER_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		SAVE_FPU_CALLER_REGS_03
#else
#endif
	.endm

	.macro RESTORE_FPU_CALLER_REGS_00
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_CALLER_REGS_01
		RESTORE_FPU_CALLER_REGS_00
	.endm

	.macro RESTORE_FPU_CALLER_REGS_02
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
	.endm

	.macro RESTORE_FPU_CALLER_REGS_03
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd17, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd19, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd21, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd23, [$sp], 8
	.endm

	.macro pop_fpu_caller
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		RESTORE_FPU_CALLER_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		RESTORE_FPU_CALLER_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		RESTORE_FPU_CALLER_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		RESTORE_FPU_CALLER_REGS_03
#else
#endif
	.endm

	/* FPU Callee registers */
	.macro SAVE_FPU_CALLEE_REGS_00
		addi    $sp, $sp, -8
		fsdi    $fd3, [$sp+0]
	.endm

	.macro SAVE_FPU_CALLEE_REGS_01
		addi    $sp, $sp, -8
		fsdi.bi $fd7, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd5, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi    $fd3, [$sp+0]
	.endm

	.macro SAVE_FPU_CALLEE_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd9, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd7, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd5, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi    $fd3, [$sp+0]
	.endm

	.macro SAVE_FPU_CALLEE_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd31, [$sp], -8
		fsdi.bi $fd30, [$sp], -8
		fsdi.bi $fd29, [$sp], -8
		fsdi.bi $fd28, [$sp], -8
		fsdi.bi $fd27, [$sp], -8
		fsdi.bi $fd26, [$sp], -8
		fsdi.bi $fd25, [$sp], -8
		fsdi.bi $fd24, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd9, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd7, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd5, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi    $fd3, [$sp+0]
	.endm

	.macro push_fpu_callee
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		SAVE_FPU_CALLEE_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		SAVE_FPU_CALLEE_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		SAVE_FPU_CALLEE_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		SAVE_FPU_CALLEE_REGS_03
#else
#endif
	.endm

	.macro RESTORE_FPU_CALLEE_REGS_00
		fldi.bi $fd3, [$sp], 8
	.endm

	.macro RESTORE_FPU_CALLEE_REGS_01
		fldi.bi $fd3, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd5, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd7, [$sp], 8
	.endm

	.macro RESTORE_FPU_CALLEE_REGS_02
		fldi.bi $fd3, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd5, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd7, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd9, [$sp], 8
		fldi.bi $fd10, [$sp], 8
	.endm

	.macro RESTORE_FPU_CALLEE_REGS_03
		fldi.bi $fd3, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd5, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd7, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd9, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd24, [$sp], 8
		fldi.bi $fd25, [$sp], 8
		fldi.bi $fd26, [$sp], 8
		fldi.bi $fd27, [$sp], 8
		fldi.bi $fd28, [$sp], 8
		fldi.bi $fd29, [$sp], 8
		fldi.bi $fd30, [$sp], 8
		fldi.bi $fd31, [$sp], 8
	.endm

	.macro pop_fpu_callee
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		RESTORE_FPU_CALLER_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		RESTORE_FPU_CALLER_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		RESTORE_FPU_CALLER_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		RESTORE_FPU_CALLER_REGS_03
#else
#endif
	.endm

	/* CALLER Frame */
	.macro CALLER_SAVE, nested
		pushm	$r15,$r30	/* full: 16 gpr, reduce: 4 gpr*/
		.if (1 == \nested)
		mfsr	$r1, $IPC
		mfsr	$r2, $IPSW
#ifdef __NDS32_EXT_IFC__
		mfusr	$r3, $IFC_LP
		pushm	$r1, $r4	/* $r4 is dummy to keep stack pointer 8-byte aligned */
#else
		pushm	$r1, $r2
#endif
		.else
#ifdef __NDS32_EXT_IFC__
		mfusr	$r1, $IFC_LP
		pushm	$r1, $r2	/* $r2 is dummy to keep stack pointer 8-byte aligned */
#endif
		.endif
		push_fpu_caller
	.endm

	.macro CALLER_RESTORE, nested
		pop_fpu_caller
		setgie.d
		dsb
		.if (1 == \nested)
#ifdef __NDS32_EXT_IFC__
		popm	$r1, $r4
		mtusr	$r3, $IFC_LP
#else
		popm	$r1, $r2
#endif
		mtsr	$r1, $IPC
		mtsr	$r2, $IPSW
		.else
#ifdef __NDS32_EXT_IFC__
		popm	$r1, $r2
		mtusr	$r1, $IFC_LP
#endif
		.endif
		popm	$r15,$r30
		popm	$r0, $r5
	.endm

	/* CALLEE Frame */
	.macro CALLEE_SAVE
		push_fpu_callee
		pushm	$r6, $r14                /* full: 9 gpr, reduce: 5 gpr*/
	.endm

	.macro CALLEE_RESTORE
		popm	$r6, $r14
		pop_fpu_callee
	.endm

	/*
	 * Nested IRQ Save/Restore
	 */
	.macro IRQ_SAVE_ALL                      /* Nested IRQ */
		CALLER_SAVE 1
		mfsr	$r1, $PSW
		addi	$r1, $r1, #-2            /* Descend interrupt level */
		mtsr	$r1, $PSW
	.endm

	.macro IRQ_RESTORE_ALL
		CALLER_RESTORE 1
	.endm

	/*
	 * Non-Nested IRQ Save/Restore
	 */
	.macro IRQ_HIPRI_SAVE_ALL                /* Non-Nested IRQ */
		CALLER_SAVE 0
	.endm

	.macro IRQ_HIPRI_RESTORE_ALL
		CALLER_RESTORE 0
	.endm

	/*
         * The SAVE/RESTORE macros for syscall.
         * Since we have the syscall handler to be equal to lowest
	 * priproty interrupt and to be reentrant, it is necessady
         * to do $IPC/$IPSW saving/restoring and descending interrupt
         * level. Because the reentrant may go beyond the HW nested
         * interruption transition level.
         */
	.macro SYSCALL_SAVE_ALL
		pushm	$r15,$r30                /* full: 16 gpr, reduce: 4 gpr*/
		mfsr	$r1, $IPC
		addi	$r1, $r1, #4             /* Save $IPC + 4 for return */
		mfsr	$r2, $IPSW
#ifdef __NDS32_EXT_IFC__
		mfusr	$r3, $IFC_LP
		pushm	$r1, $r4                 /* $r4 is dummy to keep stack pointer 8-byte aligned */
#else
		pushm	$r1, $r2
#endif
		push_fpu_caller

		/* Descend interrupt level & set PSW.CPL to 3 */
		mfsr	$r1, $PSW
		addi	$r1, $r1, #-2            /* Descend interrupt level */
		movi	$r2, #~PSW_mskCPL
		and	$r1, $r1, $r2
		movi	$r2, #(3 << PSW_offCPL)  /* PSW.CPL = 3 */
		or	$r1, $r1, $r2
		mtsr	$r1, $PSW
	.endm

	.macro SYSCALL_RESTORE_ALL
		CALLER_RESTORE 1
	.endm

#endif	// __IRQ_INC__
