#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "tgtdefs.h"
#include "vusb.h"
#include "avrdbg.h"
#include "avrutils.h"
#include "hw_setup.h"
#include "rf_protocol.h"
#include "rf_dngl.h"
#include "keycode.h"
#include "text_message.h"
#include "reports.h"

void init_hw(void)
{
	// set the LEDs as outputs
	SetBit(DDR(LED1_PORT), LED1_BIT);
	SetBit(DDR(LED2_PORT), LED2_BIT);
	SetBit(DDR(LED3_PORT), LED3_BIT);
}

int	main(void)
{
	_delay_ms(200);		// wait for the voltage levels to stabilize
	
	init_hw();
	dbgInit();
	rf_dngl_init();
	vusb_init();

	sei();

	const uint8_t RECV_BUFF_SIZE = 32;
	uint8_t recv_buffer[RECV_BUFF_SIZE];
	uint8_t bytes_received;

	bool keyboard_report_ready = false;
	bool consumer_report_ready = false;
	bool idle_elapsed = false;
	
	dprint("dongle online\n");
	
	for (;;)
	{
		idle_elapsed = vusb_poll();

		// try to read the recv buffer
		bytes_received = rf_dngl_recv(recv_buffer, RECV_BUFF_SIZE);

		if (bytes_received)
		{
			// we have new data, so what is it?
			if (recv_buffer[0] == MT_KEY_STATE)
			{
				process_key_state_msg(recv_buffer, bytes_received);

				consumer_report_ready = true;
				keyboard_report_ready = true;
			} else if (recv_buffer[0] == MT_TEXT) {
				process_text_msg(recv_buffer, bytes_received);
			}
		}

		if (!keyboard_report_ready  &&  !msg_empty())
		{
			reset_keyboard_report();

			static uint8_t prev_keycode = KC_NO;
			// get the next char from the stored text message
			uint8_t c = msg_peek();
			uint8_t new_keycode = get_keycode_for_char(c);

			// if the keycode is different than the previous
			// otherwise just send an empty report to simulate key went up
			if (new_keycode != prev_keycode  ||  new_keycode == KC_NO)
			{
				usb_keyboard_report.keys[0] = new_keycode;
				usb_keyboard_report.modifiers = get_modifiers_for_char(c);
				
				msg_pop();	// remove char from the buffer
			} else {
				new_keycode = KC_NO;
			}
			
			keyboard_report_ready = true;
			
			prev_keycode = new_keycode;		// remember for later
		}

		// send the keyboard report
        if (usbInterruptIsReady()  &&  (keyboard_report_ready  ||  idle_elapsed))
		{
            usbSetInterrupt((void*) &usb_keyboard_report, sizeof usb_keyboard_report);
			keyboard_report_ready = false;
			
			vusb_reset_idle();
		}

		// send the audio and media controls report
        if (usbInterruptIsReady3()  &&  (consumer_report_ready  ||  idle_elapsed))
		{
            usbSetInterrupt3((void*) &usb_consumer_report, sizeof usb_consumer_report);
			consumer_report_ready = false;
		}
	}

	return 0;
}
