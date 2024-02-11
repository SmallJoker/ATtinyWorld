#include "PWM_ADC.h"
#include <avr/io.h>
#include <util/delay.h> // for _delay_ms()

// TODO: Find a faster search strategy
// Lower delay = more jitter.
#define DELAY_US (1120) // 2 * R * C ≃ 1.12 ms

#define PIN_0A_IN (1 << PB3)
#define PIN_0B_IN (1 << PD4)

// Channel A
#define PWM_ADC_0A_READ (PINB & PIN_0A_IN)
u8 PWM_ADC_Read_0A()
{
	if (PWM_ADC_0A_READ) {
		// Go up
		while (PWM_ADC_0A_READ && OCR0A < 255) {
			OCR0A += 1; // Threshold
			_delay_us(DELAY_US);
		}
		return OCR0A - 1;
	} else {
		// Go down
		while (!PWM_ADC_0A_READ && OCR0A > 0) {
			OCR0A -= 1; // Threshold
			_delay_us(DELAY_US);
		}
		return OCR0A + 1;
	}
}

// Channel B
#define PWM_ADC_0B_READ (PIND & PIN_0B_IN)
u8 PWM_ADC_Read_0B()
{
	if (PWM_ADC_0B_READ) {
		// Go up
		while (PWM_ADC_0B_READ && OCR0B < 255) {
			OCR0B += 1; // Threshold
			_delay_us(DELAY_US);
		}
		return OCR0B - 1;
	} else {
		// Go down
		while (!PWM_ADC_0B_READ && OCR0B > 0) {
			OCR0B -= 1; // Threshold
			_delay_us(DELAY_US);
		}
		return OCR0B + 1;
	}
}

void PWM_ADC_Setup()
{
#if F_CPU != 8000000
#error Calculations assume 8 MHz clock
#endif

	// Freq: fosc / (N * 256)

	// PB2: OC0A (Page 54)
	DDRB |= (1 << PB2);
	TCCR0A |= (0b10 << COM0A0); // Output=1 when below threshold (Page 73)
	TCCR0A |= (0b11 << WGM00); // Fast PWM. WGM02=0 default (Page 75)
	TCCR0B |= (0b001 << CS00); // N = 1  -->  31.25 kHz

	// PD5: OC0B
	DDRD |= (1 << PD5);
	TCCR0A |= (0b10 << COM0B0); // Output=1 when below threshold (Page 74)

	OCR0A = OCR0B = 0x80; // 50% initial duty-cycle

	// Inputs
	DDRB &= ~PIN_0A_IN;
	DDRD &= ~PIN_0B_IN;
}
