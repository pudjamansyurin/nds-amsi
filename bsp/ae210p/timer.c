/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "timer.h"

#include "ae210p.h"
#define DEV_PIT AE210P_PIT

#define PIT_CHNCTRL_CLK_EXTERNAL        (0 << 3)
#define PIT_CHNCTRL_CLK_PCLK            (1 << 3)
#define PIT_CHNCTRL_MODEMASK            0x07
#define PIT_CHNCTRL_TMR_32BIT           1
#define PIT_CHNCTRL_TMR_16BIT           2
#define PIT_CHNCTRL_TMR_8BIT            3
#define PIT_CHNCTRL_PWM                 4
#define PIT_CHNCTRL_MIXED_16BIT         6
#define PIT_CHNCTRL_MIXED_8BIT          7

static void timer_init_irqchip(void)
{
	/* set TIMER1 priority to lowest */
	// __nds32__set_int_priority(IRQ_PIT_VECTOR, 3);

	/* set UART priority higher than TIMER1 if we need UART output in Timer IRQ */
	// __nds32__set_int_priority(IRQ_UART2_VECTOR, 2);

	/* enable HW# (PIT) */
	__nds32__enable_int(IRQ_PIT_VECTOR);
}

void timer_init(void)
{
	/* Disable PIT */
	DEV_PIT->CHNEN = 0;

	/* Set PIT control mode */
	DEV_PIT->CHANNEL[0].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[1].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[2].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[3].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);

	/* Clear and disable interrupt */
	DEV_PIT->INTEN = 0;
	DEV_PIT->INTST = -1;

	timer_init_irqchip();
}

void timer_start(unsigned int tmr)
{
	if (tmr < 4)
		DEV_PIT->CHNEN |= (0x1 << (4 * (tmr)));
}

void timer_stop(unsigned int tmr)
{
	if (tmr < 4)
		DEV_PIT->CHNEN &= ~(0x1 << (4 * (tmr)));
}

unsigned int timer_read(unsigned int tmr)
{
	if (tmr < 4)
		return	(DEV_PIT->CHANNEL[tmr].RELOAD - DEV_PIT->CHANNEL[tmr].COUNTER);
	else
		return 0;
}

void timer_set_period(unsigned int tmr, unsigned int period)
{
	if (tmr < 4)
		DEV_PIT->CHANNEL[tmr].RELOAD = period;
}

void timer_irq_enable(unsigned int tmr)
{
	if (tmr < 4)
		DEV_PIT->INTEN |= (0x1 << (4 * (tmr)));
}

void timer_irq_disable(unsigned int tmr)
{
	if (tmr < 4)
		DEV_PIT->INTEN &= ~(0x1 << (4 * (tmr)));
}

void timer_irq_clear(unsigned int tmr)
{
	if (tmr < 4)
		DEV_PIT->INTST = 0xF << (4 * (tmr));
}

unsigned int timer_irq_status(unsigned int tmr)
{
	return (DEV_PIT->INTST & (0xF << (4 * (tmr))));
}

unsigned int sec_to_tick(unsigned int sec)
{
	return sec * PCLKFREQ;
}

unsigned int msec_to_tick(unsigned int msec)
{
	return msec * (PCLKFREQ / 1000);
}

unsigned int usec_to_tick(unsigned int usec)
{
	return usec * (PCLKFREQ / 1000000);
}

