#pragma once

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;

#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1];

#define RINGBUFFER_NEW(NAME, LEN) \
	struct { \
		u8 head : 4; \
		volatile u8 tail : 4; \
		u8 data[LEN]; \
	} NAME = {0, 0, {}}; \
	STATIC_ASSERT(sizeof(NAME) == (LEN) + 1, unexpected_struct_size); \
	STATIC_ASSERT(LEN <= 16, buffer_too_long);

// For GPIOR0
enum GlobalFlag0 {
	GF0_ERR = 0x01, // error in previous function call
	GF0_USART_RX_OVERFLOW = 0x10,
	GF0_USART_TX_OVERFLOW = 0x20,
};
