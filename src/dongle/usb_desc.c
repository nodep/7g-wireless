#include <stdint.h>
#include <stdbool.h>

#include "usb.h"

__code const usb_dev_desc_t usb_dev_desc =
{
	sizeof(usb_dev_desc_t),
	USB_DESC_DEVICE, 
	0x0110,			// bcdUSB
	0,				// bDeviceClass		- deferred to interface
	0,				// bDeviceSubclass
	0,				// bDeviceProtocol
	USB_EP0_SIZE,	// bMaxPacketSize0
	0x40aa,			// idVendor			- some unknown vendor id
	0x9003,			// idProduct
	0x0001,			// bcdDevice
	1,				// iManufacturer
	2,				// iProduct
	3,				// iSerialNumber
	1,				// bNumConfigurations
};

// the default keyboard descriptor - compatible with keyboard boot protocol 
// taken from the HID Descriptor Tool
__code const uint8_t usb_keyboard_report_descriptor[USB_KBD_HID_REPORT_DESC_SIZE] =
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
	0xc0,				// END_COLLECTION
};

// media control report
__code const uint8_t usb_consumer_report_descriptor[USB_CONS_HID_REPORT_DESC_SIZE] =
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

__code const usb_conf_desc_keyboard_t usb_conf_desc = 
{
	// configuration descriptor
	{
		sizeof(usb_conf_desc_t),
		USB_DESC_CONFIGURATION,
		sizeof(usb_conf_desc_keyboard_t),
		2,		// bNumInterfaces
		1,		// bConfigurationValue
		2,		// iConfiguration
		0x80,	// bmAttributes - bus powered, no remote wakeup
		25,		// bMaxPower == 50mA
	},
	
	// keyboard interface descriptor
	{
		sizeof(usb_if_desc_t),
		USB_DESC_INTERFACE,
		0,		// bInterfaceNumber
		0,		// bAlternateSetting
		1,		// bNumEndpoints
		3,		// bInterfaceClass		- HID
		1,		// bInterfaceSubClass  	- Boot
		1,		// bInterfaceProtocol	- Keyboard
		0,		// iInterface
	},
	// HID descriptor
	{
		sizeof(usb_hid_desc_t),
		USB_DESC_HID,
		0x0111,					// bcdHID
		0,						// bCountryCode
		1,						// bNumDescriptors
		USB_DESC_HID_REPORT,	// bDescriptorType_HID
		sizeof(usb_keyboard_report_descriptor),	// wDescriptorLength
	},
	// endpoint descriptor EP1IN
	{
		sizeof(usb_ep_desc_t),
		USB_DESC_ENDPOINT,
		0x81,				// bEndpointAddress
		USB_EP_TYPE_INT,	// bmAttributes
		USB_EP1_SIZE,		// wMaxPacketSize
		1,					// bInterval		10ms
	},
	
	// consumer interface descriptor
	{
		sizeof(usb_if_desc_t),
		USB_DESC_INTERFACE,
		1,		// bInterfaceNumber
		0,		// bAlternateSetting
		1,		// bNumEndpoints
		3,		// bInterfaceClass		- HID
		0,		// bInterfaceSubClass
		0,		// bInterfaceProtocol
		0,		// iInterface
	},
	// HID descriptor
	{
		sizeof(usb_hid_desc_t),
		USB_DESC_HID,
		0x0111,					// bcdHID
		0,						// bCountryCode
		1,						// bNumDescriptors
		USB_DESC_HID_REPORT,	// bDescriptorType_HID
		sizeof(usb_consumer_report_descriptor),	// wDescriptorLength
	},
	// endpoint descriptor EP1IN
	{
		sizeof(usb_ep_desc_t),
		USB_DESC_ENDPOINT,
		0x82,				// bEndpointAddress
		USB_EP_TYPE_INT,	// bmAttributes
		USB_EP2_SIZE,		// wMaxPacketSize
		10,					// bInterval		10ms
	}
};

// string table 
__code const uint8_t usb_string_desc_0[] = {0x04, 0x03, 0x09, 0x04};

__code const uint16_t usb_string_desc_1[] =
{
	0x0300 | sizeof(usb_string_desc_1),		// string descriptor ID and length
	'F','e','r','e','n','c',' ','S','z','i','l','i'
};

__code const uint16_t usb_string_desc_2[] =
{
	0x0300 | sizeof(usb_string_desc_2),		// string descriptor ID and length
	'7','G',' ','W','i','r','e','l','e','s','s',' ','K','e','y','b','o','a','r','d'
};

__code const uint16_t usb_string_desc_3[] =
{
	0x0300 | sizeof(usb_string_desc_3),		// string descriptor ID and length
	'2','0','1','4','-','0','2','-','2','2'
};
