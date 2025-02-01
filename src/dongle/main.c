#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg24lu1.h"

#include "keycode.h"
#include "text_message.h"
#include "reports.h"
#include "rf_protocol.h"
#include "rf_dngl.h"
#include "nrfutils.h"

#include "usb.h"
#include "usb_regs.h"

#include "nrfdbg.h"
#include "leds.h"

__xdata void* memcpy_X(__xdata void* dest, __xdata const void* src, size_t count)
{
	__xdata char* dst8 = (__xdata char*)dest;
	__xdata const char* src8 = (__xdata const char*)src;

	while (count--)
		*dst8++ = *src8++;

	return dest;
}

void* memcpy_P(__xdata void* dest, __code const void* src, size_t count)
{
	char* dst8 = (char*)dest;
	char* src8 = (char*)src;

	while (count--)
		*dst8++ = *src8++;

	return dest;
}

#define RECV_BUFF_SIZE		32

void main()
{
	bool keyboard_report_ready = false;
	bool consumer_report_ready = false;

	uint8_t prev_keycode = KC_NO;
	uint8_t modifier_step = 0;
	
	__xdata uint8_t recv_buffer[RECV_BUFF_SIZE];
	__xdata uint8_t bytes_received;
	
	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	
	LED_off();

	usbInit();
	//dbgInit();
	
	rf_dngl_init();

	reset_keyboard_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		//dbgPoll();	// send chars from the uart TX buffer
		
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
			// peek the next char from the stored text message
			uint8_t c = msg_peek();
			uint8_t new_keycode = get_keycode_for_char(c);
			uint8_t new_modifiers = get_modifiers_for_char(c);

			reset_keyboard_report();

			// Linux seems to be a bit finicky when it comes to the order in the which the keys and
			// modifiers are received. So we have to do a keystroke sequence to get the desired result
	        if (new_modifiers != 0)
	        {
				if (modifier_step == 0)
				{
					// no keys are down
	                ++modifier_step;
				}
	            else if (modifier_step == 1)
	            {
					// sends KC_NO and the modifier
	                usb_keyboard_report.modifiers = new_modifiers;
	                ++modifier_step;
	            }
	            else if (modifier_step == 2)
	            {
					// sends the keystroke and the modifier
	                usb_keyboard_report.keys[0] = new_keycode;
	                usb_keyboard_report.modifiers = new_modifiers;
	                ++modifier_step;
	            }
	            else if (modifier_step == 3)
	            {
					// sends KC_NO and the modifier
	                usb_keyboard_report.modifiers = new_modifiers;
	                ++modifier_step;
	            }
	            else if (modifier_step == 4)
				{
					// no keys are down
	                modifier_step = 0;
	                msg_pop();	// remove char from the buffer
				}
	        }
	        else if (new_keycode != prev_keycode  ||  new_keycode == KC_NO)
	        {
	            usb_keyboard_report.keys[0] = new_keycode;
	            usb_keyboard_report.modifiers = new_modifiers;

	            msg_pop();	// remove char from the buffer
	        } else {
	            new_keycode = KC_NO;
	        }			
			prev_keycode = new_keycode;
			keyboard_report_ready = true;
		}
		
		// send the report if the endpoint is not busy
		if ((in1cs & 0x02) == 0   &&   (keyboard_report_ready  ||  usbHasIdleElapsed()))
		{
			// copy the keyboard report into the endpoint buffer
			in1buf[0] = usb_keyboard_report.modifiers;
			in1buf[1] = 0;
			in1buf[2] = usb_keyboard_report.keys[0];
			in1buf[3] = usb_keyboard_report.keys[1];
			in1buf[4] = usb_keyboard_report.keys[2];
			in1buf[5] = usb_keyboard_report.keys[3];
			in1buf[6] = usb_keyboard_report.keys[4];
			in1buf[7] = usb_keyboard_report.keys[5];

			// send the data on it's way
			in1bc = 8;
			
			keyboard_report_ready = false;
		}

		// send the consumer report if the endpoint is not busy
		if ((in2cs & 0x02) == 0   &&   (consumer_report_ready  ||  usbHasIdleElapsed()))
		{
			in2buf[0] = usb_consumer_report;
			in2bc = 1;
		
			consumer_report_ready = false;
		}
	}
}
