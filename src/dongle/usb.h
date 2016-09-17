#pragma once

// standard request codes
#define USB_REQ_GET_STATUS			0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE			0x03
#define USB_REQ_SET_ADDRESS			0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0a
#define USB_REQ_SET_INTERFACE		0x0b
#define USB_REQ_SYNCH_FRAME			0x0c

// HID request codes
#define USB_REQ_HID_GET_REPORT		0x01
#define USB_REQ_HID_GET_IDLE		0x02
#define USB_REQ_HID_GET_PROTOCOL	0x03
#define USB_REQ_HID_SET_REPORT		0x09
#define USB_REQ_HID_SET_IDLE		0x0a
#define USB_REQ_HID_SET_PROTOCOL	0x0b

// descriptor types
#define USB_DESC_DEVICE				0x01
#define USB_DESC_CONFIGURATION		0x02
#define USB_DESC_STRING				0x03
#define USB_DESC_INTERFACE			0x04
#define USB_DESC_ENDPOINT			0x05
#define USB_DESC_DEVICE_QUAL		0x06
#define USB_DESC_OTHER_SPEED_CONF	0x07
#define USB_DESC_INTERFACE_POWER	0x08

#define USB_DESC_HID				0x21
#define USB_DESC_HID_REPORT			0x22
#define USB_DESC_HID_PHYS			0x23

// endpoint types
#define USB_EP_TYPE_CTRL		0x00
#define USB_EP_TYPE_ISO			0x01
#define USB_EP_TYPE_BULK		0x02
#define USB_EP_TYPE_INT			0x03

// clear hsnak bit
#define USB_EP0_HSNAK()		ep0cs = 0x02
// set stall and dstall bits to stall during setup data transaction
#define USB_EP0_STALL()		ep0cs = 0x11

// interrupt codes
#define INT_SUDAV		0x00
#define INT_SOF			0x04
#define INT_SUTOK		0x08
#define INT_SUSPEND		0x0C
#define INT_USBRESET	0x10
#define INT_EP0IN		0x18
#define INT_EP0OUT		0x1C
#define INT_EP1IN		0x20
#define INT_EP1OUT		0x24
#define INT_EP2IN		0x28
#define INT_EP2OUT		0x2C
#define INT_EP3IN		0x30
#define INT_EP3OUT		0x34
#define INT_EP4IN		0x38
#define INT_EP4OUT		0x3C
#define INT_EP5IN		0x40
#define INT_EP5OUT		0x44

typedef enum
{
	ATTACHED,
	POWERED,
	DEFAULT,
	ADDRESSED,
	CONFIGURED,
	SUSPENDED
} usb_state_t;

typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
} usb_request_t;

typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint8_t		wValueLSB;
	uint8_t		wValueMSB;
	uint8_t		wIndexLSB;
	uint8_t		wIndexMSB;
	uint8_t		wLengthLSB;
	uint8_t		wLengthMSB;
} usb_request_value_t;

typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint8_t		descIndex;
	uint8_t		descType;
	uint16_t	langID;
	uint8_t		lengthLSB;
	uint8_t		lengthMSB;
} usb_req_std_get_desc_t;

typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint8_t		descIndex;
	uint8_t		descType;
	uint8_t		interface;
	uint8_t		filler;
	uint16_t	wLength;
} usb_req_hid_get_desc_t;

typedef struct
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	bcdUSB;
	uint8_t		bDeviceClass;
	uint8_t		bDeviceSubClass;
	uint8_t		bDeviceProtocol;
	uint8_t		bMaxPacketSize0;
	uint16_t	idVendor;
	uint16_t	idProduct;
	uint16_t	bcdDevice;
	uint8_t		iManufacturer;
	uint8_t		iProduct;
	uint8_t		iSerialNumber;
	uint8_t		bNumConfigurations;
} usb_dev_desc_t;

typedef struct
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	wTotalLength;
	uint8_t		bNumInterfaces;
	uint8_t		bConfigurationValue;
	uint8_t		iConfiguration;
	uint8_t		bmAttributes;
	uint8_t		bMaxPower;
} usb_conf_desc_t;

typedef struct
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bInterfaceNumber;
	uint8_t		bAlternateSetting;
	uint8_t		bNumEndpoints;
	uint8_t		bInterfaceClass;
	uint8_t		bInterfaceSubClass;
	uint8_t		bInterfaceProtocol;
	uint8_t		iInterface;
} usb_if_desc_t;

typedef struct
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bEndpointAddress;
	uint8_t		bmAttributes;
	uint16_t	wMaxPacketSize;
	uint8_t		bInterval;
} usb_ep_desc_t;

typedef struct
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	bcdHID;
	uint8_t		bCountryCode;
	uint8_t		bNumDescriptors;
	uint8_t		bDescriptorType_HID;
	uint16_t	wDescriptorLength;
} usb_hid_desc_t;

//
// the descriptors for this project
//

typedef struct
{
	usb_conf_desc_t	conf;

	usb_if_desc_t	if1;
	usb_hid_desc_t	hid1;
	usb_ep_desc_t	ep1in;

	usb_if_desc_t	if2;
	usb_hid_desc_t	hid2;
	usb_ep_desc_t	ep2in;
} usb_conf_desc_keyboard_t;

#define USB_STRING_DESC_COUNT			4
#define USB_KBD_HID_REPORT_DESC_SIZE	0x3f
#define USB_CONS_HID_REPORT_DESC_SIZE	0x2d

extern __code const usb_conf_desc_keyboard_t usb_conf_desc;
extern __code const usb_dev_desc_t usb_dev_desc;
extern __code const uint8_t  usb_string_desc_0[];
extern __code const uint16_t usb_string_desc_1[];
extern __code const uint16_t usb_string_desc_2[];
extern __code const uint16_t usb_string_desc_3[];
extern __code const uint8_t usb_keyboard_report_descriptor[USB_KBD_HID_REPORT_DESC_SIZE];
extern __code const uint8_t usb_consumer_report_descriptor[USB_CONS_HID_REPORT_DESC_SIZE];

void usbInit(void);
void usbPoll(void);

bool usbHasIdleElapsed(void);

#define CAPS_LOCK_MASK		0x01
#define NUM_LOCK_MASK		0x02
#define SCROLL_LOCK_MASK	0x04

__xdata extern uint8_t usbIdleRate;				// in 4 ms units

// endpoint buffer sizes
#define USB_EP0_SIZE	0x40
#define USB_EP1_SIZE	0x08
#define USB_EP2_SIZE	0x08
