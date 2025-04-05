#include "DUAL_SLOPE_ADC.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define PINB_CTRL_0 (1 << PB2)
#define PINB_IN_0   (1 << PB3)

// This function is inlined
static void my_delay(u16 count)
{
	// Instructions:
	//    nop
	//    sbiw
	//    brbc
	while (count --> 0) {
		asm("nop");
	}
}

u8 Dual_Slope_ADC_Read()
{
	/*
	DELAY_FACTOR: time interval of single steps

	RAMP1_STEPS: count of intervals to wait
		A) Higher values give a better resolution
		B) Must be <255 to allow ramp 2 to be longer (for Vin close to 50%)

	Known good combinations:
		0x90 & 128 works well for 330 nF * 39 kOhm
		0x20 & 200 works well for 330 nF * 11 kOhm

	Timing trade-off:
		A) High amplitudes reduce input bias errors.
		B) Low amplitudes allow faster sampling due to faster settle time.
	*/
#define DELAY_FACTOR (0x20)
#define RAMP1_STEPS (200)

	// 1: first ramp
	u8 dir = !!(PINB & PINB_CTRL_0); // this input is more stable
	// ^ 0: if low, do ramp up. 1: if high, do ramp down
	if (dir) {
		// High input --> ramp down by applying high ctrl voltage
		PORTB |= PINB_CTRL_0;
	} else  {
		PORTB &= ~PINB_CTRL_0;
	}
	DDRB |= PINB_CTRL_0; // output

	my_delay(RAMP1_STEPS * DELAY_FACTOR);

	// 2: second ramp until input voltage is reached
	PORTB ^= PINB_CTRL_0;
	// Wait until we reached the same condition again
	u8 ramp2_t = 0;
	while (1) {
		if (dir == !!(PINB & PINB_IN_0))
			break;
		// Compensate clock cycles used by checks (try & error)
		my_delay(1 * (DELAY_FACTOR - 2));
		ramp2_t++;
		if (!ramp2_t)
			break;
	}
#undef DELAY_FACTOR

	// 3: Idle again
	DDRB  &= ~PINB_CTRL_0; // input
	PORTB &= ~PINB_CTRL_0; // disable pull-up

	/*
		Calculate the input voltage based on the following formula:

		V_charge * t_charge + V_discharge * t_discharge
		------------------------------------------------
		(          t_charge +               t_discharge)

		... + including integer division error correction

		dir = 1  ->  V_charge = 255,  V_discharge =   0
		dir = 0  ->  V_charge =   0,  V_discharge = 255
	*/
	u16 t_total = (u16)RAMP1_STEPS + (u16)ramp2_t;
	u16 val = ((u16)(dir ? RAMP1_STEPS : ramp2_t) * 255 + (t_total / 2)) / t_total;
	return val; // range [0, 255]
}

void Dual_Slope_ADC_Setup()
{
	// All inputs by default

	// Pull-ups disabled by default. Do nothing.
	if (0) {
		PORTB &= ~PINB_CTRL_0;
		PORTB &= ~PINB_IN_0;
	}
}
