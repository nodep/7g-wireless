#pragma once

#include "tgtdefs.h"

enum msg_type_t
{
	// normal message payload (keyboard -> dongle)
	MT_KEY_STATE = 1,	// state of the keys
	MT_TEXT,			// text to be sent to the host and sent to the host as keystrokes
	
	// ACK payload (dongle -> keyboard)
	MT_LED_STATUS,			// update the status of the LEDs
	MT_TEXT_BUFF_FREE,		// number of free chars in the message text buffer on the dongle
};

// communication address
#define NRF_ADDR_SIZE	5
extern const /*__FLASH_ATTR*/ uint8_t KeyBrdAddr[NRF_ADDR_SIZE];
extern const /*__FLASH_ATTR*/ uint8_t DongleAddr[NRF_ADDR_SIZE];

// the maximum number of keys that the one packet will carry
#define MAX_KEYS		6

// the nRF channel that we are communicating on
#define CHANNEL_NUM		110

// the bits in the consumer report (audio and media controls)
#define FN_MUTE_BIT			0
#define FN_VOL_DOWN_BIT		1
#define FN_VOL_UP_BIT		2
#define FN_PLAY_PAUSE_BIT	3
#define FN_PREV_TRACK_BIT	4
#define FN_NEXT_TRACK_BIT	5

typedef struct
{
	uint8_t		msg_type;		// == MT_KEY_STATE
	uint8_t		modifiers;		// bitfield
	uint8_t		consumer;		// audio and media control key states in a bitfield
	uint8_t		keys[MAX_KEYS];
} rf_msg_key_state_report_t;

#define MAX_TEXT_LEN	30

typedef struct
{
	uint8_t		msg_type;		// == MT_TEXT
	uint8_t		msg_id;			// increments on every text message
								// used to ignore text messages that have been resent
	char		text[MAX_TEXT_LEN];
} rf_msg_text_t;


typedef struct
{
	uint8_t		msg_type;		// == MT_LED_STATUS
	uint8_t		led_status;
} rf_msg_led_status_t;


typedef struct
{
	uint8_t		msg_type;		// == MT_TEXT_BUFF_FREE
	uint8_t		bytes_free;
	uint8_t		bytes_capacity;
} rf_msg_text_buff_state_t;

/*
// this one is only used for tests
typedef struct
{
	uint8_t		msg_type;		// == MT_TEST
	uint16_t	id;				// packet ID
	uint8_t		sleep_cnt;		// num of sleep cycles
	uint8_t		plos;			// nRF OBSERVE.PLOS
	uint16_t	lost_cnt;		// manual lost counter
	uint16_t	total_arc;		// sum of nRF OBSERVE.ARC
	uint16_t	time_taken;		// in multiples of Timer2 resolution
	uint8_t		tmr_prescaler;	// the Timer2 prescale
} rf_msg_rep_test_t;		// sizeof() must be <= 32
*/