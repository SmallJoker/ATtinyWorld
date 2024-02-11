#pragma once

/*
	Channel A:
		PB2 : PWM output (to RC)
		PB3 : Digital in (comparator out)
	Channel B:
		PD5 : PWM output (to RC)
		PD4 : Digital in (comparator out)
*/

#include "datalib.h"

void PWM_ADC_Setup();
u8 PWM_ADC_Read_0A();
u8 PWM_ADC_Read_0B();
