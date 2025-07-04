#include "USART_RS232_TX_BUF.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef USART_TX_BUFFER_SIZE
	#define USART_TX_BUFFER_SIZE 5
#endif

#if USART_TX_BUFFER_SIZE > 1
	#define HAVE_BUFFER
	#define USART_T_BUSY (1 << TXCIE)
#endif

#ifdef HAVE_BUFFER
RINGBUFFER_NEW(usart_tx, USART_TX_BUFFER_SIZE)
#endif

void USART_Transmit(u8 byte)
{
#ifdef HAVE_BUFFER
	if (!(UCSRB & USART_T_BUSY)) {
		// Tx is idling. Send instantly.
		UCSRB |= USART_T_BUSY;
		UDR = byte;
		return;
	}

	// Starts with index 1
	u8 newhead = usart_tx.head + 1;
	if (newhead == USART_TX_BUFFER_SIZE)
		newhead = 0;

	if (newhead == usart_tx.tail)
		GPIOR0 |= GF0_USART_TX_OVERFLOW;

	// Transfer until a byte is free
	// !! "usart_tx.tail" must be volatile !!
	while (newhead == usart_tx.tail);
	usart_tx.data[usart_tx.head = newhead] = byte;
	return;
#endif
	// else
	while (!(UCSRA & (1 << UDRE))); // wait
	UDR = byte;
}

#ifdef HAVE_BUFFER
ISR(USART_TX_vect)
{
	// TXC is cleared automatically (Page 120)

	if (usart_tx.head == usart_tx.tail) {
		UCSRB &= ~USART_T_BUSY;
		return; // Done. Disable interrupt
	}

	if (++usart_tx.tail == USART_TX_BUFFER_SIZE)
		usart_tx.tail = 0;
	UDR = usart_tx.data[usart_tx.tail];
}
#endif

#ifdef USART_DO_DUMMY_MIRROR
ISR(USART_RX_vect)
{
	// Mirror mirror!
	USART_Transmit(UDR);
}
#endif

void USART_Setup()
{
#if F_CPU != 8000000
#error Calculations assume 8 MHz clock
#endif

	// ==> Baud Rate (Page 113)
	// UBRR = fosc / (8 * BAUD) - 1
	// BAUD = fosc / (8 * (UBRR + 1))
	UCSRA |= 1 << U2X; // Divisor 16 -> 8
#if USART_SPEED == 12
	// 1200 BAUD = 832.33333 = 0x340.555
	UBRRH = 0x3;
	UBRRL = 0x40;
#elif USART_SPEED == 384
	// 38'400 BAUD = 25.042
	UBRRH = 0;
	UBRRL = 25;
#elif USART_SPEED == 768
	// 76'800 BAUD = 12.020833
	UBRRH = 0;
	UBRRL = 12;
#else
	#error Cannot handle specified USART_SPEED value
#endif

	// ==> Format data (Page 132)
	UCSRB |= 0 << UCSZ2; // 8-Bit (default)
	// 1 Stop bit
	UCSRC |= 0b10 << UPM0 // Even parity
		| 0b011 << UCSZ0; // 8-Bit (default)
	// Tx & Rx Start
	UCSRB |= 1 << TXEN // Tx start
		| 1 << RXEN    // Rx start
#if USART_RX_BUFFER_SIZE > 0
		| 1 << RXCIE   // Rx interrupt
#endif
	;

	// Tx interrupt is set in USART_Transmit
}
