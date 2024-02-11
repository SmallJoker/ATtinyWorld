#include "USART_RS232_RX_BUF.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef USART_RX_BUFFER_SIZE
	#define USART_RX_BUFFER_SIZE 5
#endif

#ifndef USART_RX_DEVICEID
	#error missing USART_RX_DEVICEID macro
#endif

#if USART_RX_BUFFER_SIZE < 1
	#error USART_RX_BUFFER_SIZE must be >= 1
#endif

RINGBUFFER_NEW(usart_rx, USART_RX_BUFFER_SIZE)

u8 USART_Count()
{
	s8 num = usart_rx.head - usart_rx.tail;
	if (num < 0)
		num += USART_RX_BUFFER_SIZE;
	return num;
}

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

void USART_Discard()
{
	usart_rx.head = usart_rx.tail;
}


ISR(USART_RX_vect)
{
	u8 val = UDR;

	if (UCSRA & ((1 << FE) | (1 << DOR) | (1 << UPE))) {
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
