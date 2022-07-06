! -----------------------------------------------------------------------------
! irq.S
!
! Copyright (c) 2005-2017, Andes Techonology Corporation
! All rights reserved.
! -----------------------------------------------------------------------------

#include "ae210p.h"
#include "irq.inc"

!********************************************************************************************************
!	Setup HW Interrupt vector handler
!********************************************************************************************************

	.macro  SET_HWISR num, handler
	.global interrupt_HW\num
	.set    interrupt_HW\num, \handler
	.endm

	! Setup the AE210P HW interrupt vector handler

	SET_HWISR IRQ_RTCPERIOD_VECTOR, interrupt_hipri_vsr
	SET_HWISR IRQ_RTCALARM_VECTOR,  interrupt_hipri_vsr
	SET_HWISR IRQ_PIT_VECTOR,       interrupt_comm_vsr
	SET_HWISR IRQ_SPI1_VECTOR,      interrupt_comm_vsr
	SET_HWISR IRQ_SPI2_VECTOR,      interrupt_comm_vsr
	SET_HWISR IRQ_I2C_VECTOR,       interrupt_comm_vsr
	SET_HWISR IRQ_GPIO_VECTOR,      interrupt_hipri_vsr
	SET_HWISR IRQ_UART1_VECTOR,     interrupt_comm_vsr
	SET_HWISR IRQ_UART2_VECTOR,     interrupt_comm_vsr
	SET_HWISR IRQ_DMA_VECTOR,       interrupt_comm_vsr
	SET_HWISR IRQ_BMC_VECTOR,       interrupt_comm_vsr

!********************************************************************************************************
!       Highest priority HW Interrupt vector service routine (Non-Nested interrupt)
!********************************************************************************************************

interrupt_hipri_vsr:
	! We enter here with the orginal r0~r5 is saved and
	! r0 is HW interrup vector number.

	IRQ_HIPRI_SAVE_ALL

	! The entire CPU state is now stashed on the stack,
	! and the stack is also 8-byte alignment.
	! We can call C program based interrupt handler now.
	la      $r1, ISR_TABLE
	lw      $r1, [$r1+($r0<<2)]             ! ISR function pointer
	jral    $r1

	IRQ_HIPRI_RESTORE_ALL

	iret


!********************************************************************************************************
!       HW Interrupt vector service routine (Nested interrupt)
!********************************************************************************************************

interrupt_comm_vsr:
	! We enter here with the orginal r0~r5 is saved and
	! r0 is HW interrup vector number.

	IRQ_SAVE_ALL

	! allow nested now
	setgie.e

	! The entire CPU state is now stashed on the stack,
	! and the stack is also 8-byte alignment.
	! We can call C program based interrupt handler now.
	la      $r1, ISR_TABLE
	lw      $r1, [$r1+($r0<<2)]             ! ISR function pointer
	jral    $r1                             ! Call ISR

	setgie.d
	dsb

	IRQ_RESTORE_ALL

	iret


!********************************************************************************************************
!       Interrupt default handler
!********************************************************************************************************

	! Macro to define default handlers
	.macro WEAK_DEFAULT weak_sym, default_handler
	.weak \weak_sym
	.set \weak_sym ,\default_handler
	.endm

	! Default irq handler
default_irq_handler:
	b       default_irq_handler

	! Define default handler to be weak symbol.
	! They can be overwritten by other handlers
	WEAK_DEFAULT rtc_period_irq_handler,   default_irq_handler
	WEAK_DEFAULT rtc_alarm_irq_handler,    default_irq_handler
	WEAK_DEFAULT pit_irq_handler,          default_irq_handler
	WEAK_DEFAULT spi0_irq_handler,         default_irq_handler
	WEAK_DEFAULT spi1_irq_handler,         default_irq_handler
	WEAK_DEFAULT i2c_irq_handler,          default_irq_handler
	WEAK_DEFAULT gpio_irq_handler,         default_irq_handler
	WEAK_DEFAULT uart0_irq_handler,        default_irq_handler
	WEAK_DEFAULT uart1_irq_handler,        default_irq_handler
	WEAK_DEFAULT dma_irq_handler,          default_irq_handler
	WEAK_DEFAULT bmc_irq_handler,          default_irq_handler
	WEAK_DEFAULT swi_irq_handler,          default_irq_handler
	WEAK_DEFAULT ldma_irq_handler,         default_irq_handler


!********************************************************************************************************
!       Interrupt handler Table
!********************************************************************************************************

	.section .data
	.align  2

	! These tables contain the isr pointers used to deliver interrupts
	.global ISR_TABLE
ISR_TABLE:
	.long   rtc_period_irq_handler          !  0: RTC period
	.long   rtc_alarm_irq_handler           !  1: RTC alarm
	.long   pit_irq_handler                 !  2: PIT
	.long   spi0_irq_handler                !  3: SPI1
	.long   spi1_irq_handler                !  4: SPI2
	.long   i2c_irq_handler                 !  5: I2C
	.long   gpio_irq_handler                !  6: GPIO
	.long   uart0_irq_handler               !  7: UART1
	.long   uart1_irq_handler               !  8: UART2
	.long   dma_irq_handler                 !  9: DMA
	.long   bmc_irq_handler                 ! 10: BMC
	.long   swi_irq_handler                 ! 11: SWINT
	.long   ldma_irq_handler                ! 12: CPU_LDMA
	.long   0                               ! 13: EXT_INT_0
	.long   0                               ! 14: EXT_INT_1
	.long   0                               ! 15: EXT_INT_2
	.long   0                               ! 16: EXT_INT_3
	.long   0                               ! 17: EXT_INT_4
	.long   0                               ! 18: EXT_INT_5
	.long   0                               ! 19: EXT_INT_6
	.long   0                               ! 20: EXT_INT_7
	.long   0                               ! 21: EXT_INT_8
	.long   0                               ! 22: EXT_INT_9
	.long   0                               ! 23: EXT_INT_10
	.long   0                               ! 24: EXT_INT_11
	.long   0                               ! 25: EXT_INT_12
	.long   0                               ! 26: EXT_INT_13
	.long   0                               ! 27: EXT_INT_14
	.long   0                               ! 28: EXT_INT_15
	.long   0                               ! 29: EXT_INT_16
	.long   0                               ! 30: EXT_INT_17
	.long   0                               ! 31: EXT_INT_18
