#pragma once

#include "datalib.h"

/*
	8 bytes
	Even parity
	1 Stop-Bit
*/

/*
	Required: USART_SPEED - transfer speed
		-  12 :  1200 Baud
		- 384 : 38400 Baud
		- 768 : 76800 Baud (unstable)

	Opt-in: USART_DO_DUMMY_MIRROR (default: undefined)
		Sends back any incoming data

	Opt-in: USART_TX_BUFFER_SIZE (default: 5)
		Custom TX buffer size
*/


void USART_Transmit(u8 byte);
void USART_Setup();
