#pragma once

#include <avr/io.h>

// Pin mapping
enum {
	PA_OUT_EN = 1 << PIN0,
	PA_FAN_EN = 1 << PIN1,

	PB_PWM_0A         = 1 << PIN2, // used by ADC
	PB_D_OUT_CONDUCTS = 1 << PIN3,
	PB_D_IN_CONDUCTS  = 1 << PIN4,

	PD_DCDC_EN   = 1 << PIN5,
	PD_nOPAMP_ON = 1 << PIN6,
};
