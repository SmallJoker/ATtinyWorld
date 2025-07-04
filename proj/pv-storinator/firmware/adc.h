#pragma once

#include "datalib.h" // u8

enum ADC_Channel {
	ADCC_VBAT,
	ADCC_TH1,
	ADCC_TH2,
	ADCC_max
};

#if 0 // Enable averaging

// Multiplier to reduce errors caused by fixed point calculations.
// Used for `g_adc_values`. 32 for 8 samples, 64 for 4.
// (255 * 32) + (255 * 32) * 7 = 65280 (max. value in avg calc formula)
#define ADC_MULT (32)

// Moving average, 8 samples.
extern u16 g_adc_values[ADCC_max];
#endif

void PWM_ADC_Setup(void);
void PWM_ADC_Start(enum ADC_Channel which);
_Bool PWM_ADC_Step(enum ADC_Channel which);
