#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "tgtdefs.h"
#include "usbdrv.h"
#include "vusb.h"
#include "rf_dngl.h"
#include "rf_protocol.h"
#include "hw_setup.h"
#include "reports.h"
#include "avrutils.h"

uint8_t vusb_idle_rate;				// in 4 ms units - set by SET_IDLE
uint8_t vusb_idle_counter;

uint8_t vusb_curr_protocol;			// this one's a little pointless because in our case both the boot
									// protocol and the report protocol are the same, but we'll support
									// it just to be nice


uint8_t vusb_expect_data = 0;		// used by usbFunctionSetup to send messages to usbFunctionWrite

#define CFG_INTERFACE_KEYBOARD_NAME		'7','G',' ','K','e','y','b','o','a','r','d'
#define CFG_INTERFACE_KEYBOARD_SZ   	11

#define CFG_INTERFACE_CONSUMER_NAME		'7','G',' ','M','e','d','i','a',' ','C','o','n','t','r','o','l'
#define CFG_INTERFACE_CONSUMER_SZ   	16


// the default keyboard descriptor - compatible with keyboard boot protocol 
// taken from the HID Descriptor Tool
const PROGMEM char keyboard_report_descriptor[] =
{
	0x05, 0x01,			// USAGE_PAGE (Generic Desktop)
	0x09, 0x06,			// USAGE (Keyboard)
	0xa1, 0x01,			// COLLECTION (Application)
	0x05, 0x07,			//		USAGE_PAGE (Keyboard)
	0x19, 0xe0,			//		USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,			//		USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x01,			//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x08,			//		REPORT_COUNT (8)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)
	0x95, 0x05,			//		REPORT_COUNT (5)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x05, 0x08,			//		USAGE_PAGE (LEDs)
	0x19, 0x01,			//		USAGE_MINIMUM (Num Lock)
	0x29, 0x05,			//		USAGE_MAXIMUM (Kana)
	0x91, 0x02,			//		OUTPUT (Data,Var,Abs)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x75, 0x03,			//		REPORT_SIZE (3)
	0x91, 0x03,			//		OUTPUT (Cnst,Var,Abs)
	0x95, 0x06,			//		REPORT_COUNT (6)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x65,			//		LOGICAL_MAXIMUM (101)
	0x05, 0x07,			//		USAGE_PAGE (Keyboard)
	0x19, 0x00,			//		USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,			//		USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,			//		INPUT (Data,Ary,Abs)
	0xc0				// END_COLLECTION
};

// media control report
const PROGMEM char consumer_report_descriptor[] =
{
	0x05, 0x0c,			// Usage Page (Consumer Devices)	
	0x09, 0x01,			// Usage (Consumer Control)	
	0xa1, 0x01,			// Collection (Application)	
	0x15, 0x00,			//		Logical Minimum (0)	
	0x25, 0x01,			//		Logical Maximum (1)	
	0x09, 0xe2,			//		Usage (Mute)
	0x75, 0x01,			//		Report Size (1)
	0x95, 0x01,			//		Report Count (1)
	0x81, 0x06,			//		Input (Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)
	0x09, 0xea,			//		Usage (Volume Decrement)	
	0x09, 0xe9,			//		Usage (Volume Increment)	
	0x95, 0x02,			//		Report Count (2)
	0x81, 0x02,			//		Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x09, 0xcd,			//		Usage (Play/Pause)
	0x95, 0x01,			//		Report Count (1)
	0x81, 0x02,			//		Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x09, 0xb6,			//		Usage (Scan Previous Track)
	0x09, 0xb5,			//		Usage (Scan Next Track)
	0x95, 0x02,			//		Report Count (2)
	0x81, 0x62,			//		Input (Data,Var,Abs,NWrp,Lin,NPrf,Null,Bit)
	0x95, 0x02,			//		Report Count (2)
	0x81, 0x01,			//		Input (Cnst,Ary,Abs)
	0xc0				// End Collection
};

// contains the configuration descriptor and all the interface, HID and endpoint descritors as well
const PROGMEM char usbDescriptorConfiguration[] =
{
	// Configuration descriptor
	9,								// length of descriptor in bytes
	USBDESCR_CONFIG,    			// descriptor type
	9 + (9+9+7) + (9+9+7), 0,		// total length of data returned (including inlined descriptors)
	2,								// number of interfaces in this configuration
	1,								// index of this configuration
	0,								// configuration name string index
	(1 << 7),						// attributes
	USB_CFG_MAX_BUS_POWER/2,		// max USB current in 2mA units


	//
	// Keyboard interface
	//

	// Interface descriptor
	9,								// sizeof(usbDescrInterface): length of descriptor in bytes
	USBDESCR_INTERFACE,				// descriptor type: interface
	0,								// index of this interface
	0,								// alternate setting for this interface
	USB_CFG_HAVE_INTRIN_ENDPOINT,	// endpoints excl 0: number of endpoint descriptors to follow
	USB_CFG_INTERFACE_CLASS,		// CLASS: HID
	USB_CFG_INTERFACE_SUBCLASS,		// SUBCLASS: keyboard
	USB_CFG_INTERFACE_PROTOCOL,     // PROTOCOL: boot
	4,								// string index for interface

	// HID descriptor
	9,								// sizeof(usbDescrHID): length of descriptor in bytes
	USBDESCR_HID,					// descriptor type: HID
	0x01, 0x01,						// BCD representation of HID version
	0x00,							// target country code
	0x01,							// number of HID Report (or other HID class) Descriptor infos to follow
	USBDESCR_HID_REPORT,			// descriptor type: HID report
	sizeof keyboard_report_descriptor, 0,	// total length of report descriptor
	
	// Endpoint descriptor
	7,								// sizeof(usbDescrEndpoint)
	USBDESCR_ENDPOINT,				// descriptor type = endpoint
	(char) 0x81,					// IN endpoint number 1
	0x03,							// attrib: Interrupt endpoint
	8, 0,							// maximum packet size
	USB_CFG_INTR_POLL_INTERVAL,		// must be at least 10ms


	//
	// Consumer interface
	//
	
	// Interface descriptor
	9,								// sizeof(usbDescrInterface): length of descriptor in bytes
	USBDESCR_INTERFACE,				// descriptor type
	1,								// index of this interface
	0,								// alternate setting for this interface
	USB_CFG_HAVE_INTRIN_ENDPOINT3,	// endpoints excl 0: number of endpoint descriptors to follow
	0x03,							// CLASS: HID
	0,								// SUBCLASS: none
	0,								// PROTOCOL: none
	5,								// string index for interface
	
	// HID descriptor
	9,								// sizeof(usbDescrHID): length of descriptor in bytes
	USBDESCR_HID,					// descriptor type: HID
	0x01, 0x01,						// BCD representation of HID version
	0x00,							// target country code
	0x01,							// number of HID Report (or other HID class) Descriptor infos to follow
	USBDESCR_HID_REPORT,			// descriptor type: HID report
	sizeof consumer_report_descriptor, 0,	// total length of report descriptor

	// Endpoint descriptor
	7,									// sizeof(usbDescrEndpoint)
	USBDESCR_ENDPOINT,					// descriptor type = endpoint
	(char) (0x80 | USB_CFG_EP3_NUMBER),	// IN endpoint number 3
	0x03,								// attrib: Interrupt endpoint
	8, 0,								// maximum packet size
	USB_CFG_INTR_POLL_INTERVAL, 		// must be at least 10ms
};

const PROGMEM uint16_t
keyboard_interface_name[] =
{
	USB_STRING_DESCRIPTOR_HEADER( CFG_INTERFACE_KEYBOARD_SZ ),
	CFG_INTERFACE_KEYBOARD_NAME
},

consumer_interface_name[] =
{
	USB_STRING_DESCRIPTOR_HEADER( CFG_INTERFACE_CONSUMER_SZ ),
	CFG_INTERFACE_CONSUMER_NAME
};

void vusb_init(void)
{
	// inits the timer used for idle rate
    // a rate of 12M/(1024 * 256) = 45.78 Hz (period = 21845us)
	TCCR0B = _BV(CS02) | _BV(CS00);
#define TMR1US	21845L

	// a rate of 12M/(64 * 256) = 732,42 Hz (1365us)
//	TCCR0B = _BV(CS01) | _BV(CS00);
//#define TMR1US	1365L

#define OVF2MS(tmr)		((uint16_t)(( (tmr) * TMR1US) / 1000))
#define OVF2US(tmr)		((uint16_t)( (tmr) * TMR1US))

	usbInit();

	usbDeviceDisconnect();	// enforce re-enumeration, do this while interrupts are disabled!
	_delay_ms(260);			// fake USB disconnect for > 250 ms
	usbDeviceConnect();

	vusb_idle_rate = 0;
	vusb_curr_protocol = 1;	// report protocol
	
	// clear the reports
	usb_consumer_report = 0;
	reset_keyboard_report();
}

bool vusb_poll(void)
{
	usbPoll();

	bool ret_val = false;
	
	// take care of the idle rate
	if (vusb_idle_rate  &&  (TIFR0  &  _BV(TOV0)))		// timer overflow?
	{
		TIFR0 = _BV(TOV0);

		if (vusb_idle_rate != 0)
		{
			if (vusb_idle_counter > 4)
			{
				vusb_idle_counter -= 5;	// 22 ms in units of 4 ms
			} else {
				vusb_idle_counter = vusb_idle_rate;
				ret_val = true;
			}
		}
	}
	
	return ret_val;
}

void vusb_reset_idle(void)
{
	vusb_idle_counter = vusb_idle_rate;
	TIFR0 = _BV(TOV0);
	TCNT0 = 0;
}

uchar usbFunctionSetup(uchar data[8])
{
	usbRequest_t* rq = (usbRequest_t*) data;

    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS)	// class request type
	{
        if (rq->bRequest == USBRQ_HID_GET_REPORT)
		{
			SetBit(PORT(LED2_PORT), LED2_BIT);
			
			// which interface is this for?
			if (rq->wIndex.word == 1)				// keyboard interface
			{
				usbMsgPtr = (usbMsgPtr_t) &usb_keyboard_report;
				return sizeof usb_keyboard_report;
			} else if (rq->wIndex.word == 2)	{	// consumer interface
				usbMsgPtr = (usbMsgPtr_t) &usb_consumer_report;
				return 1;
			}

		} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {

            usbMsgPtr = (usbMsgPtr_t) &vusb_idle_rate;
            return 1;
			
        } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {

            vusb_idle_rate = rq->wValue.bytes[1];
			
        } else if(rq->bRequest == USBRQ_HID_SET_REPORT) {

            // Report Type: 0x02(Out)/ReportID: 0x00(none) && Interface: 0(keyboard)
            if (rq->wValue.word == 0x0200  &&  rq->wIndex.word == 0)
                 vusb_expect_data = rq->wLength.word;

            return USB_NO_MSG; // to get data in usbFunctionWrite

		} else if(rq->bRequest == USBRQ_HID_GET_PROTOCOL) {

            usbMsgPtr = (usbMsgPtr_t) &vusb_curr_protocol;
            return 1;

        } else if(rq->bRequest == USBRQ_HID_SET_PROTOCOL) {

			SetBit(PORT(LED3_PORT), LED3_BIT);
			
			// here the bios is usually setting the boot protocol
			// by having vusb_curr_protocol == 0
            vusb_curr_protocol = rq->wValue.bytes[1];
		}
    }

	return 0;
}

// V-USB calls this function when we've got data from the PC.
// In our case this will always be the state of the LEDs
uchar usbFunctionWrite(uchar *data, uchar len)
{
	if (vusb_expect_data == 0)
		return -1;

	// swap the CAPS and SCROLL bits because the controller
	// has these the other way around than the report
	rf_msg_led_status_t msg;
	msg.msg_type = MT_LED_STATUS;
	msg.led_status = (data[0] & 1)
						| ((data[0] & 2) ? 4 : 0)
						| ((data[0] & 4) ? 2 : 0);

	// queue the status which will be sent with the next ACK payload
	rf_dngl_queue_ack_payload(&msg, sizeof msg);

	vusb_expect_data = 0;

	return 1;
}

usbMsgLen_t usbFunctionDescriptor(struct usbRequest* rq)
{
	usbMsgLen_t len = 0;

	if (rq->wValue.bytes[1] == USBDESCR_HID_REPORT)
	{
		if (rq->wIndex.word == 0)	// interface index
		{
			usbMsgPtr = (usbMsgPtr_t) keyboard_report_descriptor;
			len = sizeof keyboard_report_descriptor;
		} else if (rq->wIndex.word == 1) {
			usbMsgPtr = (usbMsgPtr_t) consumer_report_descriptor;
			len = sizeof consumer_report_descriptor;
		}

	} else if (rq->wValue.bytes[1] == USBDESCR_STRING) {

		if (rq->wValue.bytes[0] == 4)			// keyboard interface name
		{
			usbMsgPtr = (usbMsgPtr_t) keyboard_interface_name;
			len = sizeof keyboard_interface_name;
		} else if (rq->wValue.bytes[0] == 5) {	// consumer interface name
			usbMsgPtr = (usbMsgPtr_t) consumer_interface_name;
			len = sizeof consumer_interface_name;
		}
	}

	return len;
}
