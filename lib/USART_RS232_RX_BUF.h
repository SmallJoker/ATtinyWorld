#pragma once

#include "datalib.h"

/*
	Required: USART_RX_DEVICEID
		marks the start of incoming data
		must be an even number, 1 byte
		0 disables this feature

	Opt-in: USART_RX_BUFFER_SIZE (default: 5)
		Custom RX buffer size
*/

u8 USART_Count();
u8 USART_Read(u8 do_advance);
void USART_Discard();
