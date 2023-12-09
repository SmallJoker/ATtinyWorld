/*
	PD0 : RX
	PD1 : TX

	PB0 : LED (active high)

	See make.sh for speed configuration
	See headers for byte format
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> // _delay_ms()

#include "USART_RS232_RX_BUF.h"
#include "USART_RS232_TX_BUF.h"

#define LED (1 << PB0)

int main()
{
	USART_Setup();

	DDRB |= LED;

	SREG |= 0x80; // Global interrupt enable

	USART_Transmit('!'); // START

	while (1) {
		_delay_ms(500);

		if (USART_Count() >= 2) {
			PORTB ^= LED;
			// Send back status + 2 bytes at once
			USART_Transmit(GPIOR0);
			for (u8 i = 0; i < 2; ++i) {
				u8 tmp = USART_Read(0);
				USART_Transmit(tmp);
			}
			USART_Discard(); // leftover bytes
			GPIOR0 = 0;
		}
	}

    return 0;
}
