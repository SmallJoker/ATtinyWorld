#include "USART_RS232_RX_BUF.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef USART_RX_BUFFER_SIZE
	#define missing USART_RX_BUFFER_SIZE macro
#endif

#if USART_RX_BUFFER_SIZE > 1
	#define HAVE_BUFFER
#endif

#ifdef HAVE_BUFFER
	// Identification prefix bytes are only possible to detect when there's a buffer.
	#ifndef USART_RX_DEVICEID
		#error missing USART_RX_DEVICEID macro
	#endif

RINGBUFFER_NEW(usart_rx, USART_RX_BUFFER_SIZE)
#endif

#define USART_R_ERROR_FLAGS ((1 << FE) | (1 << DOR) | (1 << UPE))

u8 USART_Count()
{
#ifdef HAVE_BUFFER
	s8 num = usart_rx.head - usart_rx.tail;
	if (num < 0)
		num += USART_RX_BUFFER_SIZE;
	return num;
#endif
	// else: 0 or 1
	if (!(UCSRA & (1 << RXC)))
		return 0; // No byte received-

	if (UCSRA & USART_R_ERROR_FLAGS) {
		// Read from register to clear RXC (Page 123)
		// avr-gcc compiles the lines below into an `IN` instruction.
		u8 discard = UDR;
		(void)discard;
		return 0;
	}
	return 1;
}

#ifdef HAVE_BUFFER
u8 USART_Read(u8 do_advance)
{
	if (usart_rx.head == usart_rx.tail)
		return 0; // Empty

	u8 newtail = usart_rx.tail + 1;
	if (newtail == USART_RX_BUFFER_SIZE)
		newtail = 0;

	if (do_advance)
		usart_rx.tail = newtail;
	return usart_rx.data[newtail];
}
#endif
// else: read from UDR directly to avoid function call overhead

#ifdef HAVE_BUFFER
void USART_Discard()
{
	usart_rx.head = usart_rx.tail;
}

ISR(USART_RX_vect)
{
	// "Due to the buffering of the error flags, the UCSRA
	// must be read before the receive buffer (UDR)"
	const u8 status = UCSRA;
	u8 val = UDR;

	if (status & USART_R_ERROR_FLAGS) {
		// Transmission error
		USART_Discard();
		return;
	}

	if (usart_rx.head == usart_rx.tail) {
#if USART_RX_DEVICEID > 0
		// Empty. Start condition.
		if ((val ^ USART_RX_DEVICEID) > 1)
			return;
#endif
	}

	//USART_Transmit('A' + USART_Count());
	u8 newhead = usart_rx.head + 1;
	if (newhead == USART_RX_BUFFER_SIZE)
		newhead = 0;

	if (newhead == usart_rx.tail) {
		GPIOR0 |= GF0_USART_RX_OVERFLOW;
		USART_Discard();
		return; // buffer overflow
	}

	usart_rx.head = newhead;
	usart_rx.data[newhead] = val;
}
#endif
// if no buffer: use UDR directly.
