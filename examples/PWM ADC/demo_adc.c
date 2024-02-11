/*
	See PWM_ADC.h for ADC description

	PB1 : Charge pump 50% "PWM" output

	See headers for USART/RS232 format
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> // _delay_ms()

#include "USART_RS232_TX_BUF.h"
#include "PWM_ADC.h"

#define PB_CHARGE_PUMP (1 << PB1)

ISR(TIMER0_OVF_vect)
{
	// Output frequency: 31.25 kHz / 2
	PORTB ^= PB_CHARGE_PUMP;
}

int main()
{
	USART_Setup();
	PWM_ADC_Setup();

	// Charge pump
	DDRB |= PB_CHARGE_PUMP; // Output
	TIMSK |= (1 << TOIE0); // Overflow interrupt

	SREG |= 0x80; // Global interrupt enable

	USART_Transmit('!'); // START

	u8 counter = 0;
	while (1) {
		_delay_ms(1000);

		u8 value_a = PWM_ADC_Read_0A();
		u8 value_b = PWM_ADC_Read_0B();
		USART_Transmit(++counter);
		USART_Transmit(value_a);
		USART_Transmit(value_b);
	}

    return 0;
}
