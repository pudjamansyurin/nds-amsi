!********************************************************************************************************
!
!                               (c) Copyright 2005-2018, Andes Techonology
!                                          All Rights Reserved
!
!                                           NDS32 Generic Port
!                                            GNU C Compiler
!
!********************************************************************************************************
!********************************************************************************************************
! INCLUDE ASSEMBLY CONSTANTS
!********************************************************************************************************

#include <nds32_init.inc>
#include "nds32_defs.h"

#ifndef VECTOR_NUMINTRS
#define VECTOR_NUMINTRS		32
#endif

	! Macro for exception/interruption vectors

	.macro VECTOR name
	.weak   trap_\name
	.set    trap_\name, default_exception_vsr
	.align 4
__vector_\name:
	push    $r0
	la      $r0, trap_\name
	jr5     $r0
	.endm

	.macro INTERRUPT num
	.weak   interrupt_HW\num
	.set    interrupt_HW\num, default_interrupt_vsr
	.align 4
__vector_HW\num:
	pushm   $r0, $r5
	li      $r0, \num
	la      $r1, interrupt_HW\num
	jr5     $r1
	.endm

!********************************************************************************************************
!	Vector Entry Table
!********************************************************************************************************

	! Define standard NDS32 vector table entry point of
	! exception/interruption vectors

	.section .nds32_init, "ax"

	.global exception_vectors
	.global exception_vectors_end

exception_vectors:
	! Exception vectors
	b       _reset              !  (0) Trap Reset/NMI
	VECTOR  TLB_Fill            !  (1) Trap TLB fill
	VECTOR  PTE_Not_Present     !  (2) Trap PTE not present
	VECTOR  TLB_Misc            !  (3) Trap TLB misc
	VECTOR  TLB_VLPT_Miss       !  (4) Trap TLB VLPT miss
	VECTOR  Machine_Error       !  (5) Trap Machine error
	VECTOR  Debug_Related       !  (6) Trap Debug related
	VECTOR  General_Exception   !  (7) Trap General exception
	VECTOR  Syscall             !  (8) Syscall

	! Interrupt vectors
	.altmacro
	.set    irqno, 0
	.rept   VECTOR_NUMINTRS
	INTERRUPT %irqno
	.set    irqno, irqno+1
	.endr

_reset:
	! Decide whether this is an NMI or cold reset
	mfsr    $r1, $itype
	andi    $r2, $r1, ITYPE_mskETYPE
	bnez    $r2, nmi_handler    ! if $itype.etype != 0
	j       _start

	.align 4
exception_vectors_end:

!******************************************************************************************************
!	Start Entry
!******************************************************************************************************
	.section .text
	.global _start
	.type   _start, @function
_start:
	!************************** Begin of do-not-modify **************************
	! Initialize the registers used by the compiler

	nds32_init	! NDS32 startup initial macro in <nds32_init.inc>

	!*************************** End of do-not-modify ***************************

#ifdef CFG_CACHE_ENABLE
	! disable cache
	mfsr    $r0, $CACHE_CTL
	li      $r1, ~(CACHE_CTL_mskIC_EN | CACHE_CTL_mskDC_EN)
	and     $r0, $r0, $r1
	mtsr    $r0, $CACHE_CTL
#endif

	! Do system low level setup. It must be a leaf function.
	bal     _nds32_init_mem

	! System reset handler
	bal     reset_handler

	! Infinite loop, if returned accidently
1:
	b       1b

	! NMI handler symbol
	.weak   nmi_handler
	.set    nmi_handler, default_exception_vsr

	/* Default exceptions / interrupts handler */
default_exception_vsr:
default_interrupt_vsr:
die:	b       die
