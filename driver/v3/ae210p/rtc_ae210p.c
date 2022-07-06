/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "../../../driver/v3/ae210p/rtc_ae210p.h"

#include <nds32_intrinsic.h>
#include "../../../driver/v3/ae210p/config/RTE_Device.h"

#define NDS_RTC_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

// Driver Version
static const NDS_DRIVER_VERSION rtc_driver_version =
{
	NDS_RTC_API_VERSION,
	NDS_RTC_DRV_VERSION
};

NDS_DRIVER_VERSION rtc_get_version(void)
{
	return rtc_driver_version;
}

int32_t rtc_initialize(NDS_RTC_SignalEvent_t cb_event)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = AE210P_RTC->CTRL;
	Tmp_S = AE210P_RTC->STATUS;

	// disdable all interrupt and clear interrupt status
	AE210P_RTC->CTRL = (Tmp_C & (~ BITS(0,7)));
	AE210P_RTC->STATUS = (Tmp_S | BITS(2,7));

	// enable CPU IRQ0(RTC period), IRQ1(RTC alarm)
	// PSW.CPL is interrupt priority level 0~3, 0 is the highest, unable to be preemptive by another int
	__nds32__set_int_priority(NDS32_HWINT(IRQ_RTCPERIOD_VECTOR), 0);
	__nds32__set_int_priority(NDS32_HWINT(IRQ_RTCALARM_VECTOR), 0);
	__nds32__enable_int(NDS32_HWINT(IRQ_RTCPERIOD_VECTOR));
	__nds32__enable_int(NDS32_HWINT(IRQ_RTCALARM_VECTOR));
	__nds32__setgie_en();

	g_rRtc_Info.Sig_Evt_CB = cb_event;
	g_rRtc_Info.Pwr_State = NDS_POWER_FULL;

	return NDS_DRIVER_OK;
}

int32_t rtc_uninitialize(void)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = AE210P_RTC->CTRL;
	Tmp_S = AE210P_RTC->STATUS;

	// disdable all interrupt and clear interrupt status
	AE210P_RTC->CTRL = (Tmp_C & (~ BITS(0,7)));
	AE210P_RTC->STATUS = (Tmp_S | BITS(2,7));

	// disable CPU IRQ0(RTC period), IRQ1(RTC alarm)
	__nds32__disable_int(NDS32_HWINT(IRQ_RTCPERIOD_VECTOR));
	__nds32__disable_int(NDS32_HWINT(IRQ_RTCALARM_VECTOR));
	__nds32__dsb();

	g_rRtc_Info.Sig_Evt_CB = NULL;
	g_rRtc_Info.Pwr_State = NDS_POWER_OFF;
	return NDS_DRIVER_OK;
}

int32_t rtc_power_control(NDS_POWER_STATE state)
{
	g_rRtc_Info.Pwr_State = state;

	switch (state) {
	case NDS_POWER_OFF:
		break;
	case NDS_POWER_LOW:
		break;
	case NDS_POWER_FULL:
		break;
	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	return NDS_DRIVER_OK;
}

int32_t rtc_set_time(NDS_RTC_TIME* stime, uint32_t format)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_S = AE210P_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	do {
		Tmp_S = AE210P_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	} while (!(Tmp_S & STATUS_WRITEDONE));

	if (	(stime->day >= (0x1 << ATCRTC100_DAY_BITS)) ||
		(stime->hour >= 24) ||
		(stime->min >= 60)  ||
		(stime->sec >= 60)) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	// keep the current set time for reference
	g_rRtc_Info.Set_Rtc_Time.day = stime->day;
	g_rRtc_Info.Set_Rtc_Time.hour = stime->hour;
	g_rRtc_Info.Set_Rtc_Time.min = stime->min;
	g_rRtc_Info.Set_Rtc_Time.sec = stime->sec;

	Tmp_C = (stime->day << 17) | (stime->hour << 12) | (stime->min << 6) | (stime->sec << 0);

	// set to RTC HW IP
	AE210P_RTC->CNTR = Tmp_C;

	return NDS_DRIVER_OK;
}

int32_t rtc_get_time(NDS_RTC_TIME* stime, uint32_t format)
{
	uint32_t Tmp = 0;

	if (g_rRtc_Info.Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// get from RTC HW IP
	Tmp = AE210P_RTC->CNTR;

	// return the current time queried from HW
	stime->day = ((Tmp & CNTR_DAY) >> 17);
	stime->hour = ((Tmp & CNTR_HOUR) >> 12);
	stime->min = ((Tmp & CNTR_MIN) >> 6);
	stime->sec = ((Tmp & CNTR_SEC) >> 0);

	if (	(stime->day >= (0x1 << ATCRTC100_DAY_BITS)) ||
		(stime->hour >= 24) ||
		(stime->min >= 60)  ||
		(stime->sec >= 60)) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	return NDS_DRIVER_OK;
}

int32_t rtc_set_alarm(NDS_RTC_ALARM* salarm, uint32_t format)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_S = AE210P_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	do {
		Tmp_S = AE210P_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	}while(!(Tmp_S & STATUS_WRITEDONE));

	if(	(salarm->hour >= 24) ||
		(salarm->min >= 60)  ||
		(salarm->sec >= 60)) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	// keep the current alarm for reference
	g_rRtc_Info.Set_Rtc_Alarm.hour = salarm->hour;
	g_rRtc_Info.Set_Rtc_Alarm.min = salarm->min;
	g_rRtc_Info.Set_Rtc_Alarm.sec = salarm->sec;

	Tmp_C = (salarm->hour << 12) | (salarm->min << 6) | (salarm->sec << 0);

	// set to RTC HW IP
	AE210P_RTC->ALARM = Tmp_C;

	return NDS_DRIVER_OK;
}

int32_t rtc_get_alarm(NDS_RTC_ALARM* salarm, uint32_t format)
{
	uint32_t Tmp = 0;

	if (g_rRtc_Info.Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// get from RTC HW IP
	Tmp = AE210P_RTC->ALARM;

	//return the current alarm queried from HW
	salarm->hour = ((Tmp & ALARM_HOUR) >> 12);
	salarm->min = ((Tmp & ALARM_MIN) >> 6);
	salarm->sec = ((Tmp & ALARM_SEC) >> 0);

	if (	(salarm->hour >= 24) ||
		(salarm->min >= 60)  ||
		(salarm->sec >= 60)) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	return NDS_DRIVER_OK;
}

int32_t rtc_control(uint32_t control, uint32_t arg)
{
	NDS_RTC_CTRL_ID eCtrlId = (NDS_RTC_CTRL_ID)control;
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = AE210P_RTC->CTRL;
	Tmp_S = AE210P_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	do {
		Tmp_S = AE210P_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	} while (!(Tmp_S & STATUS_WRITEDONE));

	switch (eCtrlId) {
	case NDS_RTC_CTRL_NONE:
		break;
	case NDS_RTC_CTRL_EN:
		// RTC ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_RTC_EN);
		// RTC OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_RTC_EN);
		}
		break;
	case NDS_RTC_CTRL_ALARM_WAKEUP:
		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_ALARM_WAKEUP);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_ALARM_WAKEUP);
		}
		break;
	case NDS_RTC_CTRL_ALARM_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_ALARM_INT);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_ALARM_INT);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_ALARM_INT);
		}
		break;
	case NDS_RTC_CTRL_DAY_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_DAY);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_DAY);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_DAY);
		}
		break;
	case NDS_RTC_CTRL_HOUR_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_HOUR);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_HOUR);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_HOUR);
		}
		break;
	case NDS_RTC_CTRL_MIN_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_MIN);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_MIN);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_MIN);
		}
		break;
	case NDS_RTC_CTRL_SEC_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_SEC);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_SEC);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_SEC);
		}
		break;
	case NDS_RTC_CTRL_HSEC_INT:
		// write 1 clear
		AE210P_RTC->STATUS = (Tmp_S | STATUS_HSEC);

		// INT ON
		if (arg == 1) {
			AE210P_RTC->CTRL = (Tmp_C | CTRL_HSEC);
		// INT OFF
		} else if (arg == 0) {
			AE210P_RTC->CTRL = (Tmp_C & ~CTRL_HSEC);
		}
		break;
	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	return NDS_DRIVER_OK;
}

NDS_RTC_STATUS rtc_get_status(void)
{
	NDS_RTC_STATUS  rRtc_Status;

	rRtc_Status.Status = AE210P_RTC->STATUS;

	return rRtc_Status;
}

void rtc_signal_event(uint32_t event)
{
	// invoke callback function
	if (g_rRtc_Info.Sig_Evt_CB) {
		g_rRtc_Info.Sig_Evt_CB(event);
	}
}

void rtc_period_irq_handler()
{
	uint32_t Evt_Id = 0;
	uint32_t Tmp_S, Tmp_W;

	// only check day/hour/min/sec/hsec interrupt, not include alarm int
	Tmp_W = Tmp_S = (AE210P_RTC->STATUS & BITS(3,7));

	// write 1 clear all interrupt status
	AE210P_RTC->STATUS = Tmp_W;

	do {
		if (Tmp_S & STATUS_DAY) {
			Evt_Id |= NDS_RTC_EVENT_DAY_INT;
		}

		if (Tmp_S & STATUS_HOUR) {
			Evt_Id |= NDS_RTC_EVENT_HOUR_INT;
		}

		if (Tmp_S & STATUS_MIN) {
			Evt_Id |= NDS_RTC_EVENT_MIN_INT;
		}

		if (Tmp_S & STATUS_SEC) {
			Evt_Id |= NDS_RTC_EVENT_SEC_INT;
		}

		if (Tmp_S & STATUS_HSEC) {
			Evt_Id |= NDS_RTC_EVENT_HSEC_INT;
		}

		rtc_signal_event(Evt_Id);
	} while (0);
}

void rtc_alarm_irq_handler()
{
	uint32_t Evt_Id = 0;
	uint32_t Tmp_S, Tmp_W;

	// only check alarm int
	Tmp_W = Tmp_S = (AE210P_RTC->STATUS & BIT(2));

	// write 1 clear all interrupt status
	AE210P_RTC->STATUS = Tmp_W;

	do {
		if (Tmp_S & STATUS_ALARM_INT) {
			Evt_Id |= NDS_RTC_EVENT_ALARM_INT;
		}

		rtc_signal_event(Evt_Id);
	} while (0);
}

RTC_INFO g_rRtc_Info =
{
	NULL,
	{0},
	{0},
	NDS_POWER_OFF
};

NDS_DRIVER_RTC Driver_RTC =
{
	rtc_get_version,
	rtc_initialize,
	rtc_uninitialize,
	rtc_power_control,
	rtc_set_time,
	rtc_get_time,
	rtc_set_alarm,
	rtc_get_alarm,
	rtc_control,
	rtc_get_status
};
