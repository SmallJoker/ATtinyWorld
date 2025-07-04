#include <avr/interrupt.h>
#include <avr/io.h>
#include "datalib.h"
#include "adc.h"
#include "USART_RS232_RX_BUF.h"
#include "USART_RS232_TX_BUF.h"

// For debugging
#define PA_DBG_LED PA_FAN_EN
#define PA_ERR_LED PA_OUT_EN

// Pin mapping
enum {
	PA_OUT_EN = 1 << PIN0,
	PA_FAN_EN = 1 << PIN1,

	PB_D_OUT_CONDUCTS = 1 << PIN3,
	PB_D_IN_CONDUCTS  = 1 << PIN4,

	PD_DCDC_EN   = 1 << PIN5,
	PD_nOPAMP_ON = 1 << PIN6,
};

enum BatteryManagementStateMachine {
	BMSM_ENTRY,
	BMSM_DISABLE_CHARGING,
	BMSM_CHARGE_BATTERY,
	BMSM_KEEP_DISCHARGING,
	BMSM_EMERGENCY_TURN_OFF
} s_bmsm = BMSM_ENTRY;

ISR(WDT_OVERFLOW_vect)
{
	// Watchdog elapsed!
	// Resume from sleep
	MCUCR &= ~(1 << SE); // Disable sleep
	USART_Transmit('W');
}


uint32_t countdown = 0;

static void adc_state_machine()
{
	// setup

	// step

	// report done
}

static void statemachine()
{
	switch (s_bmsm) {
		case BMSM_ENTRY:
			break;
		case BMSM_DISABLE_CHARGING:
			break;
		case BMSM_CHARGE_BATTERY:
			break;
		case BMSM_KEEP_DISCHARGING:
			break;
		case BMSM_EMERGENCY_TURN_OFF:
			break;
	}
}

int main()
{
	if (0) {
		// Watchdog TODO

		MCUSR &= ~(1 << WDRF); // clear to avoid loops
		// Mode = Interrupt mode
		// 1 second timeout
		WDTCSR = 0b01000000 | (0b110 << WDP0);
	}

	USART_Setup();
	PWM_ADC_Setup();

	DDRA |= PA_DBG_LED;
	DDRA |= PA_ERR_LED;

	//TIMSK |= (1 << TOIE0); // Timer 0 Overflow interrupt

	{
		// Watchdog for sleeping
		MCUCR |= (0b000 << SM0); // idle mode (default)
		// Enter sleep mode: MCUCR |= (0b010 << SM0); asm("sleep");
	}

	SREG |= 0x80; // Global interrupt enable

	PWM_ADC_Start(ADCC_VBAT);

	USART_Transmit('!'); // START

	uint32_t counter = 0;
	while (1) {
		if (counter == 0) {
			//statemachine();
			if (USART_Count()) {
				// Basic communication test (echo)
				u8 v = UDR;
				OCR0A = v;
				USART_Transmit(v);
			}
		}

		if (counter == 0x1FFF) {
			PORTA ^= PA_DBG_LED; // blink :)
		}

		if ((counter & 0xFFF) == 0x000) {
			_Bool done = PWM_ADC_Step(ADCC_VBAT);
			if (done)
				USART_Transmit(OCR0A);
		}

		counter++;
		if (counter == 0x1FFFF)
			counter = 0;
	}
}
