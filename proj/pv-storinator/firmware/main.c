#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h> // wdt_reset
#include <avr/sleep.h> // sleep_disable
#include "datalib.h"
#include "adc.h"
#include "USART_RS232_RX_BUF.h"
#include "USART_RS232_TX_BUF.h"

// ------------------------------- Declarations

// Pin mapping
enum {
	PA_OUT_EN = 1 << PIN0,
	PA_FAN_EN = 1 << PIN1,

	PB_D_OUT_CONDUCTS = 1 << PIN3,
	PB_D_IN_CONDUCTS  = 1 << PIN4,

	PD_DCDC_EN   = 1 << PIN5,
	PD_nOPAMP_ON = 1 << PIN6,

	// For debugging
	PA_DBG_LED = PA_FAN_EN,
	PA_ERR_LED = PA_OUT_EN,
};

enum {
	ADC_VBAT_CRIT   = 100, // TODO critical low threshold
	ADC_VBAT_40PERC = 120, // TODO 40% threshold
	ADC_VBAT_FULL   = 130, // TODO (close to) 100%
};

enum BatteryManagementStateMachine {
	BMSM_ENTRY,
	BMSM_DISABLE_CHARGING,
	BMSM_CHARGE_BATTERY,
	BMSM_CHECK_BATTERY_LOW,
	BMSM_KEEP_DISCHARGING,
	BMSM_EMERGENCY_TURN_OFF
} s_bmsm = BMSM_ENTRY;


// ------------------------------- Watchdog

ISR(WDT_OVERFLOW_vect)
{
	// Watchdog with interrupt == Resume from "Power down" sleep

	USART_Transmit('W');
	USART_Transmit(0); // flush the other byte
}

// https://electronics.stackexchange.com/a/74850
static void WDT_Setup_reset_64ms()
{
	// Watchdog: Clear from previous runs
	MCUSR &= ~(1 << WDRF); // clear to avoid loops
	wdt_reset();           // re-start timer

	// WDCE must be set to manipulate the Watchdog! (Page 42)
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	//WDTCSR = 0x00; // disable watchdog

	// Set up watchdog to detect system lock-ups
	WDTCSR = (1 << WDIF)   // 1 -> clear flag
		| (1 << WDE)       // "System Reset Mode" (Table 18)
		| (0b010 << WDP0); // 64 ms (Table 19)
}

static void enter_sleep()
{
	// Setup watchdog
	{
		wdt_reset();
		// WDCE must be set to manipulate the Watchdog! (Page 42)
		WDTCSR |= (1 << WDCE) | (1 << WDE);

		// Set up watchdog to resume from sleep after X seconds
		WDTCSR = (1 << WDIF)           // 1 -> clear flag
			| (1 << WDE) | (1 << WDIE) // "Interrupt Mode" (Table 18)
			| (1 << WDP3);             // 4 seconds (Table 19)
	}

	if (1) {
		// Enable INT0 (Table 31)
		// MCUCR: keep ISC01 and ISC00 at 0 (low level detection)
		EIFR |= (1 << INTF0); // 1 -> clear flag
		MCUCR |= (0b00 << ISC00); // logical change
		GIMSK |= (1 << INT0);
	}
	// BUG FIXME: Why does INT0 not work?! Do we need an edge trigger?
	// Page 59 notes that INT0 and INT1 require an I/O clock, however
	// Table 14 does only mention INT0.
	// According to Table 7, 38400 baud implies 208 CK, which should be enough.

	ACSR |= (1 << ACD); // turn off unused comparator
	sleep_enable();
	sleep_cpu(); // wait for INT0 or watchdog
	sleep_disable();
	ACSR &= ~(1 << ACD);

	WDT_Setup_reset_64ms();
}

// ------------------------------- Other Interrupt Service Routines

ISR(INT0_vect)
{
	// Incoming UART transmission
	GIMSK &= ~(1 << INT0); // disable INT0

	USART_Transmit('I');
}

// ------------------------------- State Machines

static void adc_state_machine()
{
	// setup

	// step

	// report done
}

// returns 1 when sleep may be entered, 0 otherwise.
static _Bool statemachine()
{
	//USART_Transmit(0xF0 + s_bmsm);
	switch (s_bmsm) {
		case BMSM_ENTRY: {
			if (PINB & PB_D_OUT_CONDUCTS) {
				s_bmsm = BMSM_DISABLE_CHARGING;
				break;
			}

			// Measure battery voltage
			_Bool done = PWM_ADC_Step(ADCC_VBAT);
			if (!done)
				break; // postpone

			// Debug information
			//USART_Transmit(OCR0A);

			if (OCR0A >= ADC_VBAT_40PERC)
				PORTA |= PA_OUT_EN;

			if (OCR0A >= ADC_VBAT_FULL) {
				s_bmsm = BMSM_DISABLE_CHARGING;
				break;
			}

			// Battery yet not full
			if (PINB & PB_D_IN_CONDUCTS) {
				s_bmsm = BMSM_CHARGE_BATTERY;
			} else {
				s_bmsm = BMSM_CHECK_BATTERY_LOW;
			}
		} break;
		case BMSM_DISABLE_CHARGING:
			PIND &= ~PD_DCDC_EN;
			s_bmsm = BMSM_CHECK_BATTERY_LOW;
			break;
		case BMSM_CHECK_BATTERY_LOW:
			// Battery voltage OK?
			if (OCR0A > ADC_VBAT_CRIT) {
				s_bmsm = BMSM_KEEP_DISCHARGING;
			} else {
				s_bmsm = BMSM_EMERGENCY_TURN_OFF;
			}
			break;
		case BMSM_CHARGE_BATTERY:
			PIND |= PD_DCDC_EN;
			// TODO enable PCINT for PB_D_OUT_CONDUCTS
			return 1;
		case BMSM_KEEP_DISCHARGING:
			// do nothing
			return 1;
		case BMSM_EMERGENCY_TURN_OFF:
			if (!(PORTA & PA_OUT_EN)) {
				// TUrn off everything
				PIND &= ~PD_DCDC_EN;
				USART_Transmit('X');
				// TODO Wait 30 seconds
				PORTA &= ~PA_OUT_EN;
			}
			return 1;
	}
	return 0;
}

// ------------------------------- Main, Setup & Loop

_Bool sleep_requested = 0;
int main()
{
	WDT_Setup_reset_64ms();

	USART_Setup();
	PWM_ADC_Setup();

	DDRA |= PA_OUT_EN | PA_FAN_EN;
	//DDRB |= 0; // no outputs
	DDRD |= PD_DCDC_EN | PD_nOPAMP_ON;

	{
#if F_CPU != 8000000
#error Calculations assume 8 MHz clock
#endif
		// Main loop timing: use Timer 1
		TCCR1B = (0b001 << CS10); // clk / 1 (Table 46)
		// (no interrupt, TOIE1=0)
		OCR1 = 16000 - 2; // approx. 2 ms
	}

	{
		// Watchdog and INT0 (level) to resume from sleep
		MCUCR |= (0b001 << SM0); // Sleep mode: Power down
		// see: enter_sleep();
	}

	SREG |= 0x80; // Global interrupt enable

	// -------------------------------------

	PWM_ADC_Start(ADCC_VBAT);

	USART_Transmit('!'); // START

	uint8_t counter = 0;
	while (1) {
		{
			// Periodic execution. Wait for next cycle.
			while (!(TIFR & (1 << TOV1)));
			TCNT1 = 0;           // restart counter
			TIFR |= (1 << TOV1); // 1 -> clear flag
		}

		wdt_reset(); // Ping! We are still alive!

		if (sleep_requested) {
			sleep_requested = 0;
			enter_sleep();
		}

		//statemachine();
		if (counter == 0) {
			PINA = PA_DBG_LED; // blink :)

			//statemachine();

			if (USART_Count()) {
				const u8 v = UDR;
				//while (1) {} // simulate hang

				// Finish transmission first
				if (v == 0x55)
					sleep_requested = 1;

				// Basic communication test (echo)
				OCR0A = v;
				USART_Transmit(v);
			}
		}

		if ((counter & 0xF) == 0x000) {
			_Bool done = PWM_ADC_Step(ADCC_VBAT);
			if (done)
				USART_Transmit(OCR0A);
		}

		counter++;
		if (counter == 0x1F)
			counter = 0;
	}
}
