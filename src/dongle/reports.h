#pragma once

#include "tgtdefs.h"

void reset_keyboard_report(void);
void process_key_state_msg(__xdata const uint8_t* recv_buffer, const uint8_t bytes_received);
void process_text_msg(__xdata const uint8_t* recv_buffer, const uint8_t bytes_received);

// this is the HID report structure
// this is what the data that we send to the host is comprised of
typedef struct
{
	uint8_t	modifiers;		// bit	mod key
							// 7	LCTRL
							// 6	LSHIFT
							// 5	LALT
							// 4	LGUI
							// 3	RCTRL
							// 2	RSHIFT
							// 1	RALT
							// 0	RGUI
							
	uint8_t	unused;
	uint8_t	keys[6];		// the keycode as defined in keycode.h
	
} hid_kbd_report_t;


extern hid_kbd_report_t	usb_keyboard_report;	// the HID keyboard report
extern uint8_t			usb_consumer_report;	// sound control report
// contains the last received LED report
extern uint8_t			usb_led_report;		// bit	LED
											// 0	CAPS
											// 1	NUM
											// 2	SCROLL
