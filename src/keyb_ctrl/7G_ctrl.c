#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "hw_setup.h"
#include "avrdbg.h"
#include "avrutils.h"
#include "matrix.h"
#include "led.h"
#include "nRF24L.h"
#include "rf_protocol.h"
#include "rf_ctrl.h"
#include "keycode.h"
#include "sleeping.h"
#include "ctrl_settings.h"
#include "calibrate_rc.h"
#include "proc_menu.h"

// returns false if we should enter the menu, true if we should lock the keyboard
bool process_normal(void)
{
	bool waiting_for_all_keys_up = false;
	bool are_all_keys_up;
	bool ret_val = false;

	do {
		wait_for_matrix_change();

		// make a key state report
		rf_msg_key_state_report_t report;
		report.msg_type = MT_KEY_STATE;
		report.modifiers = 0;
		report.consumer = 0;

		uint8_t num_keys = 0;

		are_all_keys_up = true;

		if (is_pressed_keycode(KC_FMNU))
		{
			are_all_keys_up = false;

			// set the bits of the consumer byte (media keys)
			if (is_pressed_keycode(KC_F1))		report.consumer |= _BV(FN_MUTE_BIT);
			if (is_pressed_keycode(KC_F2))		report.consumer |= _BV(FN_VOL_DOWN_BIT);
			if (is_pressed_keycode(KC_F3))		report.consumer |= _BV(FN_VOL_UP_BIT);
			if (is_pressed_keycode(KC_F4))		report.consumer |= _BV(FN_PLAY_PAUSE_BIT);
			if (is_pressed_keycode(KC_F5))		report.consumer |= _BV(FN_PREV_TRACK_BIT);
			if (is_pressed_keycode(KC_F6))		report.consumer |= _BV(FN_NEXT_TRACK_BIT);

			// if only Func and Esc are pressed
			if (get_num_keys_pressed() == 2)
			{
				if (is_pressed_keycode(KC_ESC))
				{
					waiting_for_all_keys_up = true;
				} else if (is_pressed_keycode(KC_L)) {
					waiting_for_all_keys_up = true;
					ret_val = true;
				} else if (is_pressed_keycode(KC_KP_MINUS)) {

					uint8_t curr_power = get_nrf_output_power();

					if (curr_power == vRF_PWR_0DBM)
						set_nrf_output_power(vRF_PWR_M6DBM);
					else if (curr_power == vRF_PWR_M6DBM)
						set_nrf_output_power(vRF_PWR_M12DBM);
					else if (curr_power == vRF_PWR_M12DBM)
						set_nrf_output_power(vRF_PWR_M18DBM);

				} else if (is_pressed_keycode(KC_KP_PLUS)) {

					uint8_t curr_power = get_nrf_output_power();

					if (curr_power == vRF_PWR_M18DBM)
						set_nrf_output_power(vRF_PWR_M12DBM);
					else if (curr_power == vRF_PWR_M12DBM)
						set_nrf_output_power(vRF_PWR_M6DBM);
					else if (curr_power == vRF_PWR_M6DBM)
						set_nrf_output_power(vRF_PWR_0DBM);
				}
			}

		} else {

			uint8_t row, col;
			for (row = 0; row < NUM_ROWS; ++row)
			{
				for (col = 0; col < NUM_COLS; ++col)
				{
					if (is_pressed_matrix(row, col))
					{
						are_all_keys_up = false;

						uint8_t keycode = get_keycode(row, col);

						if (IS_MOD(keycode))
							report.modifiers |= _BV(keycode - KC_LCTRL);
						else if (num_keys < MAX_KEYS)
							report.keys[num_keys++] = keycode;
					}
				}
			}
		}

		// send the report and wait for ACK
		if (!rf_ctrl_send_message(&report, num_keys + 3))
			return true;

		// flush the ACK payloads
		rf_ctrl_process_ack_payloads(NULL, NULL);

	} while (!waiting_for_all_keys_up  ||  are_all_keys_up);

	return ret_val;
}

void process_lock(void)
{
	start_led_sequence(led_seq_lock);

	for (;;)
	{
		sleep_ticks(0xfe);	// long, about 62ms x 3 = 190ms
		sleep_ticks(0xfe);
		sleep_ticks(0xfe);

		if (matrix_scan()
				&&  get_num_keys_pressed() == 3
				&&  is_pressed_keycode(KC_FMNU)
				&&  (is_pressed_keycode(KC_LCTRL)  ||  is_pressed_keycode(KC_RCTRL))
				&&  (is_pressed_keycode(KC_DEL)  ||  is_pressed_keycode(KC_KP_DOT)))
		{
			break;
		}
	}

	start_led_sequence(led_seq_lock);
}

void init_hw(void)
{
	// power down everything we don't need
	power_adc_disable();	// get_battery_voltage() turns this back on and off again
	power_lcd_disable();
	//power_spi_disable();	// maybe we should power this off when not in use?
	power_timer1_disable();
	power_usart0_disable();	// init_dbg() will power on the USART if called
	SetBit(ACSR, ACD);		// analog comparator off

	// default all pins to input with pullups
	DDRA = 0;	PORTA = 0xff;
	DDRB = 0;	PORTB = 0xff;
	DDRC = 0;	PORTC = 0xff;
	DDRD = 0;	PORTD = 0xff;
	DDRE = 0;	PORTE = 0xff;
	DDRF = 0;	PORTF = 0xff;
	DDRG = 0;	PORTG = 0xff;

#ifdef DBGPRINT
	// PE0 and PE1 are used for timing and debugging
	// Note: these are the UART RX (PE0) and TX (PE1) pins

	DDRE = _BV(0) | _BV(1);
	dbgInit();
	calibrate_rc();
	dprint("i live...\n");
#else
	OSCCAL = OSCCAL_DEFAULT;
#endif

	matrix_init();
	rf_ctrl_init();
	init_leds();
	init_sleep();
}

int main(void)
{
	init_hw();	// initialize the hardware

	sei();		// enable interrupts

	// 'play' a LED sequence while waiting for the 32KHz crystal to stabilize
	start_led_sequence(led_seq_boot);
	while (are_leds_on())
		;

	for (;;)
	{
		if (process_normal()  ||  process_menu())
			process_lock();
	}

	return 0;
}
