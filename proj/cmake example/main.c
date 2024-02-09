#include <avr/interrupt.h>
#include <avr/io.h>
#include "datalib.h"

int main()
{
#if F_CPU != 1234567
	#error F_CPU
#endif
#ifndef __AVR_ATtiny2313__
	#error TYPE_MACRO
#endif

	SREG |= 0x80; // Global interrupt enable

}
