/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __GPIO_AE210P_H
#define __GPIO_AE210P_H

#include "ae210p.h"
#include "../include/Driver_GPIO.h"

// GPIO interrupt mode
#define GPIO_INTR_HIGH_LEVEL        0x2
#define GPIO_INTR_LOW_LEVEL         0x3
#define GPIO_INTR_NEGATIVE_EDGE     0x5
#define GPIO_INTR_POSITIVE_EDGE     0x6
#define GPIO_INTR_DUAL_EDGE         0x7

#define GPIO_MAX_PIN_NUM            32

// GPIO information
typedef struct _GPIO_INFO {
	NDS_GPIO_SignalEvent_t cb_event;  // event callback
} GPIO_INFO;

/*****************************************************************************
 *  HAL Level : Interrupt                                                    *
 ****************************************************************************/
#define NDS32_HWINT_ID(hw)     NDS32_INT_H##hw
#define NDS32_HWINT(hw)        NDS32_HWINT_ID(hw)

#endif /* __GPIO_AE210P_H */
