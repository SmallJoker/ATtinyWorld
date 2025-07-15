#include "adc.h"
#include "pins.h"

u8 g_adc_values[ADCC_max] = { 0x80, 0x80, 0x80 };

//#define avg_reg OCR1B

static _Bool get_value(enum ADC_Channel which)
{
	switch (which) {
		case ADCC_VBAT:
			// Internal
			return !!(ACSR & (1 << ACO));
		case ADCC_TH1:
			// TH1 pin
			return !!(PIND & (1 << PIN3));
		case ADCC_TH2:
			// TH2 pin
			return !!(PIND & (1 << PIN4));
		default:
			return 0; // invalid
	}
}

static _Bool comp_out_prev;
static u8 counter;
void PWM_ADC_Start(enum ADC_Channel which)
{
	ACSR &= ~(1 << ACD); // enable comparator

	counter = 0;
	OCR0A = g_adc_values[which] - 4; // try to approach from the bottom
	TCCR0A |= (0b10 << COM0A0); // Output=1 when below threshold (Table 34)

	// PB2: OC0A, timer 0 PWM output (Page 54)
	DDRB |= PB_PWM_0A;
}

// Assumes OCR0A is already preloaded
_Bool PWM_ADC_Step(enum ADC_Channel which)
{
	_Bool comp_out = get_value(which);
	if (comp_out) {
		if (OCR0A < 255)
			OCR0A++;
		else
			goto done;
	} else {
		if (OCR0A > 0)
			OCR0A--;
		else
			goto done;
	}

	counter += (comp_out != comp_out_prev);
	comp_out_prev = comp_out;

	if (counter >= 3) {
done:
		counter = 0;
		// The sampled value is in `OCR0A`.

#if 0
		// Average calculation (same instruction count when using `u16 avg`).
		u16 *avg = &g_adc_values[which];
		if (!*avg)
			*avg = OCR0A * ADC_MULT;
		else
			*avg = ((OCR0A * ADC_MULT) + (*avg) * 7 + 7) / 8;
#endif
		g_adc_values[which] = OCR0A;
		DDRB &= ~PB_PWM_0A; // stop PWM output
		return 1;
	}
	return 0;
}

void PWM_ADC_Setup(void)
{
#if F_CPU != 8000000
#error Calculations assume 8 MHz clock
#endif
	// Freq: fosc / (N * 256)

	DIDR = 0b11 << AIN0D; // Power saving. Disable digital inputs on PB0 & PB1.

	// PB0 & PB1: Comparator input, disable pull-up.
	PORTB &= ~(0b11 << PIN0);

	TCCR0A |= (0b11 << WGM00); // Fast PWM. (Table 39)
	// Output is configured in `PWM_ADC_Start`.

	// (WGM02 = 0 is default)
	TCCR0B |= (0b001 << CS00); // Clock Select. N = 1  -->  31.25 kHz

	//PORTB &= ~(1 << PIN2); // (default) pull down when PWM is inactive


	// Enable analog comparator
	// ACSR = 0; (default)
}
