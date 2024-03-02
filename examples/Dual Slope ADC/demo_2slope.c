/*
	PD0 : USART RX
	PD1 : USART TX
	See headers for USART/RS232 format

	PB0 : LED (active high)
	PB1 : Charge pump (output)
	PB2 : ADC Tristate CTRL
	PB3 : ADC comparator input
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> // _delay_ms()

#include "USART_RS232_RX_BUF.h"
#include "USART_RS232_TX_BUF.h"
#include "DUAL_SLOPE_ADC.h"

#define LED (1 << PB0)

/*
	Tricks

	expr > 0  -->  !!expr (-10 bytes code)
*/

#define PB_CHARGE_PUMP (1 << PB1)

ISR(TIMER0_OVF_vect)
{
	// Output frequency: 31.25 kHz / 2
	PORTB ^= PB_CHARGE_PUMP;
}

int main()
{
	USART_Setup();
	Dual_Slope_ADC_Setup();

	DDRB |= LED;
	{
		TCCR0A |= (0b11 << WGM00); // Fast PWM. WGM02=0 default (Page 75)
		TCCR0B |= (0b001 << CS00); // N = 1  -->  31.25 kHz
		TIMSK |= (1 << TOIE0); // Overflow interrupt

		DDRB |= PB_CHARGE_PUMP;
	}

	SREG |= 0x80; // Global interrupt enable

	USART_Transmit('!'); // START

	uint32_t counter = 0;
	while (1) {
		if (counter == 1) {
			PORTB ^= LED;

			// May take up to 30 ms with DELAY_FACTOR = 0x60
			u8 val = Dual_Slope_ADC_Read();
			USART_Transmit(val);
		}

		counter++;
		if (counter == 0x1FFFF)
			counter = 0;
	}

    return 0;
}
