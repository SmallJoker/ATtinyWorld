#include <avr/interrupt.h>
#include <avr/wdt.h> // wdt_reset
#include <avr/sleep.h> // sleep_disable
#include "datalib.h"
#include "adc.h"
#include "pins.h"
#include "USART_RS232_RX_BUF.h"
#include "USART_RS232_TX_BUF.h"

// ------------------------------- Declarations

enum {
	ADC_VBAT_CRIT   = 100, // TODO critical low threshold
	ADC_VBAT_40PERC = 120, // TODO 40% threshold
	ADC_VBAT_FULL   = 130, // TODO (close to) 100%
};

enum BatteryManagementStateMachine {
	BMSM_ENTRY,
	BMSM_VBAT_CHECK,
	BMSM_DISABLE_CHARGING,
	BMSM_CHARGE_BATTERY,
	BMSM_CHECK_BATTERY_LOW,
	BMSM_KEEP_DISCHARGING,
	BMSM_EMERGENCY_TURN_OFF,
	BMSM_EMERGENCY_TURN_OFF_2,

	BMSM_HELPER_DELAY,
} s_bmsm = BMSM_ENTRY, s_bmsm_delayed = BMSM_ENTRY;

enum StateMachineReturnValue {
	RV_CONTINUE, // keep running
	RV_SLEEP,    // enter power saving mode
	RV_RESET_AND_SLEEP, // reset state machine to BMSM_ENTRY and sleep
};

static _Bool sleep_requested = 0;


// ------------------------------- Watchdog

ISR(WDT_OVERFLOW_vect)
{
	// Watchdog with interrupt == Resume from "Power down" sleep

	USART_Transmit('W');
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

	// Flush pending UART data
	while (!(UCSRA & (1 << UDRE))) {}

	PORTD |= PD_nOPAMP_ON; // disable OPAMP

	// Enable INT0 (Table 31), triggered by UART RX
	{
		// Documentation BUG: Page 59 notes that INT0 and INT1 require an I/O clock
		// for edge detection, however Table 14 does only mention INT0.

		// 38400 baud implies 208 CK, which is long enough
		// to resume from Power-down (Table 7)

		// MCUCR: keep ISC01 and ISC00 at 0 (low level detection)
		EIFR |= (1 << INTF0); // 1 -> clear flag
		GIMSK |= (1 << INT0);
	}

	DDRB &= ~PB_PWM_0A; // disable PWM output
	ACSR |= (1 << ACD); // turn off unused comparator
	sleep_enable();
	sleep_cpu(); // wait for INT0 or watchdog
	sleep_disable();

	WDT_Setup_reset_64ms();
}

// ------------------------------- Other Interrupt Service Routines

ISR(INT0_vect)
{
	// Incoming UART transmission
	GIMSK &= ~(1 << INT0); // disable INT0

	USART_Transmit('I');
}

ISR(PCINT_vect)
{
	GIMSK &= ~(1 << PCIE); // disable PCINT*

	USART_Transmit('P');
}

// ------------------------------- State Machines, Helper functions

static void adc_state_machine()
{
	// setup

	// step

	// report done
}

// Unused Timer 0 output B. This saves RAM and instruction bytes.
#define delay_counter OCR0B

// returns 1 when sleep may be entered, 0 otherwise.
static enum StateMachineReturnValue statemachine()
{
	//USART_Transmit(0xF0 + s_bmsm);
	switch (s_bmsm) {
		case BMSM_ENTRY:
			if (PINB & PB_D_OUT_CONDUCTS) {
				s_bmsm = BMSM_DISABLE_CHARGING;
				break;
			}

			PWM_ADC_Start(ADCC_VBAT);
			s_bmsm = BMSM_VBAT_CHECK;
			break;
		case BMSM_VBAT_CHECK: {
			// Measure battery voltage
			_Bool done = PWM_ADC_Step(ADCC_VBAT);
			if (!done)
				break; // postpone

			// Debug information
			USART_Transmit(OCR0A);

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
			if (g_adc_values[ADCC_VBAT] > ADC_VBAT_CRIT) {
				s_bmsm = BMSM_KEEP_DISCHARGING;
			} else {
				s_bmsm = BMSM_EMERGENCY_TURN_OFF;
			}
			break;
		case BMSM_CHARGE_BATTERY:
			// In case the pin has changed in the meantime -> stop.
			if (PINB & PB_D_OUT_CONDUCTS) {
				s_bmsm = BMSM_DISABLE_CHARGING;
				break;
			}
			// TODO: Use fan for cooling, if necessary (TH1 & TH2)

			PIND |= PD_DCDC_EN;
			{
				// PCINT to disable charging when there's insufficient input voltage.
				// Prepared in `main()`. See `PCINT_vect` callback.
				GIMSK |= 1 << PCIE; // pin change interrupt enable
			}
			return RV_RESET_AND_SLEEP;
		case BMSM_KEEP_DISCHARGING:
			// do nothing
			return RV_RESET_AND_SLEEP;
		case BMSM_EMERGENCY_TURN_OFF:
			if (!(PORTA & PA_OUT_EN)) {
				// Turn off everything
				PIND &= ~PD_DCDC_EN;
				USART_Transmit('X');

				s_bmsm = BMSM_HELPER_DELAY;
				s_bmsm_delayed = BMSM_EMERGENCY_TURN_OFF_2;
				delay_counter = 8; // 32 seconds (@ 4 seconds sleep interval)
			} else {
				s_bmsm = BMSM_EMERGENCY_TURN_OFF_2;
			}
			break;
		case BMSM_EMERGENCY_TURN_OFF_2:
			PORTA &= ~PA_OUT_EN;

			// Sleep... until PB_D_IN_CONDUCTS goes to high?
			return RV_RESET_AND_SLEEP;
		case BMSM_HELPER_DELAY:
			// Used for long delays. Use Power-down cycles.
			// FIXME: This does not work properly when woken up from sleep by interrupts
			if (delay_counter) {
				delay_counter--; // maybe decrement in WDT_OVERFLOW_vect ?
				return RV_SLEEP;
			}

			s_bmsm = s_bmsm_delayed;
			break;
	}
	return RV_CONTINUE;
}

static void usart_rx_handler()
{
	if (!USART_Count())
		return;

	u8 v = UDR;

	// Finish transmission first
	switch (v) {
		case 0x54: // simulate hang
			while (1) {}
		case 0x55:
			sleep_requested = 1;
			break;
		case 0x56: // restart evaluation
			s_bmsm = BMSM_ENTRY;
			break;
		case 0x80: // State machine status request
			v = s_bmsm;
			break;
		case 0x90:
		case 0x91:
		case 0x92: // Get ADC values
			v = g_adc_values[v - 0x60];
			break;
	}

	// Basic communication test (echo)
	USART_Transmit(v);
}

// ------------------------------- Main, Setup & Loop

int main()
{
	WDT_Setup_reset_64ms();

	USART_Setup();
	PWM_ADC_Setup();

	PORTD |= PD_nOPAMP_ON; // disable OPAMP

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

	{
		// Prepare Pin Change interrupts.
		STATIC_ASSERT(PB_D_OUT_CONDUCTS == 1 << PIN3, unexpected_pin);
		PCMSK = 1 << PCINT3; // battery --> out diode conducts
	}

	SREG |= 0x80; // Global interrupt enable

	// -------------------------------------

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
			continue;
		}

		switch (statemachine()) {
			case RV_CONTINUE:
				break;
			case RV_RESET_AND_SLEEP:
				s_bmsm = BMSM_ENTRY;
				// fall-though
			case RV_SLEEP:
				sleep_requested = 1;
				break;
		}

		if (counter == 0) {
			PINA = PA_DBG_LED; // blink :)
		}

		usart_rx_handler();

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
