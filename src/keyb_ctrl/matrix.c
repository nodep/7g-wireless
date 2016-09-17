#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include "matrix.h"
#include "keycode.h"

const __flash uint8_t matrix2keycode[NUM_ROWS][NUM_COLS] = 
{
//       0        1        2        3        4        5        6        7
	{ KC_NO,   KC_NO,   KC_NLCK, KC_NO,   KC_PMNS, KC_RALT, KC_LALT, KC_NO   },		//  0
	{ KC_NO,   KC_CAPS, KC_NO,   KC_LCTL, KC_NO,   KC_NO,   KC_NO,   KC_RCTL },		//  1		
	{ KC_NO,   KC_NO,   KC_NUBS, KC_LSFT, KC_PAUS, KC_NO,   KC_NO,   KC_RSFT },		//  2
	{ KC_1,    KC_TAB,  KC_GRV,  KC_Q,    KC_ESC,  KC_NO,   KC_A,    KC_Z    },		//  3
	{ KC_UP,   KC_FMNU, KC_SPC,  KC_NUBS, KC_RGHT, KC_NO,   KC_NO,   KC_NO   },		//  4
	{ KC_LGUI, KC_NO,   KC_PENT, KC_NO,   KC_SLCK, KC_NO,   KC_NO,   KC_NO   },		//  5
	{ KC_PSLS, KC_PAST, KC_APP,  KC_P7,   KC_PSCR, KC_PPLS, KC_P4,   KC_P1   },		//  6
	{ KC_INS,  KC_DEL,  KC_F4,   KC_P9,   KC_F3,   KC_P3,   KC_P6,   KC_PDOT },		//  7
	{ KC_HOME, KC_END,  KC_F12,  KC_P8,   KC_F11,  KC_P2,   KC_P5,   KC_P0   },		//  8
	{ KC_PGUP, KC_PGDN, KC_F2,   KC_2,    KC_F1,   KC_S,    KC_W,    KC_X    },		//  9
	{ KC_9,    KC_MINS, KC_F8,   KC_O,    KC_F7,   KC_L,    KC_LBRC, KC_DOT  },		// 10
	{ KC_EQL,  KC_BSPC, KC_F6,   KC_RBRC, KC_F5,   KC_LEFT, KC_BSLS, KC_ENT  },		// 11
	{ KC_0,    KC_DOWN, KC_F10,  KC_SCLN, KC_F9,   KC_SLSH, KC_QUOT, KC_P    },		// 12
	{ KC_Y,    KC_U,    KC_7,    KC_H,    KC_6,    KC_N,    KC_J,    KC_M    },		// 13
	{ KC_R,    KC_T,    KC_5,    KC_F,    KC_4,    KC_V,    KC_G,    KC_B    },		// 14
	{ KC_I,    KC_E,    KC_8,    KC_K,    KC_3,    KC_C,    KC_D,    KC_COMM }		// 15
};

uint8_t matrix[NUM_ROWS];				// current state of the keyboard matrix
uint8_t matrix_num_keys_pressed = 0;	// contains the number of keys that are pressed

void matrix_init(void)
{
	uint8_t row;
	for (row = 0; row < NUM_ROWS; ++row)
		matrix[row] = 0;
}

bool matrix_scan(void)
{
	bool has_changes = false;
	uint8_t row, col;

	matrix_num_keys_pressed = 0;	// no keys are pressed
	
	// config ports D and A as outputs and drive them low
	DDRD = 0xff;	PORTD = 0x00;
	DDRA = 0xff;	PORTA = 0x00;

	_delay_us(3);	// wait a little for the levels to stabilize
	
	// first we want to know if any keys are pressed.
	// most of the time no key will be pressed,
	// so there's no reason to waste time scaning the entire matrix bit by bit

	// are none of the keys pressed?
	if (PINC == 0xff)
	{
		// no keys pressed - update the matrix
		for (row = 0; row < NUM_ROWS; row++)
		{
			if (matrix[row])
			{
				matrix[row] = 0;
				has_changes = true;
			}
		}

	} else {

		// at least one key is pressed - find out which one(s)
		for (row = 0; row < NUM_ROWS; row++)
		{
			// drive the outputs
			if (row < 8)
				PORTA = ~_BV(row), PORTD = 0xff;
			else
				PORTA = 0xff, PORTD = ~_BV(row - 8);

			// we have to wait a little for the levels to stabilize
			_NOP();
			_NOP();
			_NOP();
			_NOP();
			_NOP();
			_NOP();
			_NOP();
			_NOP();
			
			// sample the inputs
			uint8_t cols = ~PINC;

			// check the columns
			for (col = 0; col < NUM_COLS; col++)
			{
				// get the states
				bool curr_state = cols & _BV(col);
				bool prev_state = matrix[row] & _BV(col);
				
				// increment the number of keys pressed
				if (curr_state)
					++matrix_num_keys_pressed;

				// update the matrix if needed
				if (curr_state != prev_state)
				{
					matrix[row] ^= _BV(col);
					has_changes = true;
				}
			}
		}
	}

	// back to inputs with pull-ups
	DDRD = 0x00;	PORTD = 0xff;
	DDRA = 0x00;	PORTA = 0xff;
	
	return has_changes;
}

uint8_t get_keycode(uint8_t row, uint8_t col)
{
	uint8_t ret_val = matrix2keycode[row][col];
	 
	return ret_val;
}

// lookup for keycode -> matrix[] bit position
// a little wasteful in terms of flash space, but we've got plenty of flash,
// so it's not a problem
typedef struct
{
	uint8_t		row;
	uint8_t		mask;
} keycode2matrix_t;

const __flash keycode2matrix_t keycode2matrix[0xe8] = 
{
	{ 0, 0x00},		// 0x00
	{ 0, 0x00},		// 0x01
	{ 0, 0x00},		// 0x02
	{ 0, 0x00},		// 0x03
	{ 3, 0x40},		// 0x04  KC_A
	{14, 0x80},		// 0x05  KC_B
	{15, 0x20},		// 0x06  KC_C
	{15, 0x40},		// 0x07  KC_D
	{15, 0x02},		// 0x08  KC_E
	{14, 0x08},		// 0x09  KC_F
	{14, 0x40},		// 0x0a  KC_G
	{13, 0x08},		// 0x0b  KC_H
	{15, 0x01},		// 0x0c  KC_I
	{13, 0x40},		// 0x0d  KC_J
	{15, 0x08},		// 0x0e  KC_K
	{10, 0x20},		// 0x0f  KC_L
	{13, 0x80},		// 0x10  KC_M
	{13, 0x20},		// 0x11  KC_N
	{10, 0x08},		// 0x12  KC_O
	{12, 0x80},		// 0x13  KC_P
	{ 3, 0x08},		// 0x14  KC_Q
	{14, 0x01},		// 0x15  KC_R
	{ 9, 0x20},		// 0x16  KC_S
	{14, 0x02},		// 0x17  KC_T
	{13, 0x02},		// 0x18  KC_U
	{14, 0x20},		// 0x19  KC_V
	{ 9, 0x40},		// 0x1a  KC_W
	{ 9, 0x80},		// 0x1b  KC_X
	{13, 0x01},		// 0x1c  KC_Y
	{ 3, 0x80},		// 0x1d  KC_Z
	{ 3, 0x01},		// 0x1e  KC_1
	{ 9, 0x08},		// 0x1f  KC_2
	{15, 0x10},		// 0x20  KC_3
	{14, 0x10},		// 0x21  KC_4
	{14, 0x04},		// 0x22  KC_5
	{13, 0x10},		// 0x23  KC_6
	{13, 0x04},		// 0x24  KC_7
	{15, 0x04},		// 0x25  KC_8
	{10, 0x01},		// 0x26  KC_9
	{12, 0x01},		// 0x27  KC_0
	{11, 0x80},		// 0x28  KC_ENT
	{ 3, 0x10},		// 0x29  KC_ESC
	{11, 0x02},		// 0x2a  KC_BSPC
	{ 3, 0x02},		// 0x2b  KC_TAB
	{ 4, 0x04},		// 0x2c  KC_SPC
	{10, 0x02},		// 0x2d  KC_MINS
	{11, 0x01},		// 0x2e  KC_EQL
	{10, 0x40},		// 0x2f  KC_LBRC
	{11, 0x08},		// 0x30  KC_RBRC
	{11, 0x40},		// 0x31  KC_BSLS
	{ 0, 0x00},		// 0x32
	{12, 0x08},		// 0x33  KC_SCLN
	{12, 0x40},		// 0x34  KC_QUOT
	{ 3, 0x04},		// 0x35  KC_GRV
	{15, 0x80},		// 0x36  KC_COMM
	{10, 0x80},		// 0x37  KC_DOT
	{12, 0x20},		// 0x38  KC_SLSH
	{ 1, 0x02},		// 0x39  KC_CAPS
	{ 9, 0x10},		// 0x3a  KC_F1
	{ 9, 0x04},		// 0x3b  KC_F2
	{ 7, 0x10},		// 0x3c  KC_F3
	{ 7, 0x04},		// 0x3d  KC_F4
	{11, 0x10},		// 0x3e  KC_F5
	{11, 0x04},		// 0x3f  KC_F6
	{10, 0x10},		// 0x40  KC_F7
	{10, 0x04},		// 0x41  KC_F8
	{12, 0x10},		// 0x42  KC_F9
	{12, 0x04},		// 0x43  KC_F10
	{ 8, 0x10},		// 0x44  KC_F11
	{ 8, 0x04},		// 0x45  KC_F12
	{ 6, 0x10},		// 0x46  KC_PSCR
	{ 5, 0x10},		// 0x47  KC_SLCK
	{ 2, 0x10},		// 0x48  KC_PAUS
	{ 7, 0x01},		// 0x49  KC_INS
	{ 8, 0x01},		// 0x4a  KC_HOME
	{ 9, 0x01},		// 0x4b  KC_PGUP
	{ 7, 0x02},		// 0x4c  KC_DEL
	{ 8, 0x02},		// 0x4d  KC_END
	{ 9, 0x02},		// 0x4e  KC_PGDN
	{ 4, 0x10},		// 0x4f  KC_RGHT
	{11, 0x20},		// 0x50  KC_LEFT
	{12, 0x02},		// 0x51  KC_DOWN
	{ 4, 0x01},		// 0x52  KC_UP
	{ 0, 0x04},		// 0x53  KC_NLCK
	{ 6, 0x01},		// 0x54  KC_PSLS
	{ 6, 0x02},		// 0x55  KC_PAST
	{ 0, 0x10},		// 0x56  KC_PMNS
	{ 6, 0x20},		// 0x57  KC_PPLS
	{ 5, 0x04},		// 0x58  KC_PENT
	{ 6, 0x80},		// 0x59  KC_P1
	{ 8, 0x20},		// 0x5a  KC_P2
	{ 7, 0x20},		// 0x5b  KC_P3
	{ 6, 0x40},		// 0x5c  KC_P4
	{ 8, 0x40},		// 0x5d  KC_P5
	{ 7, 0x40},		// 0x5e  KC_P6
	{ 6, 0x08},		// 0x5f  KC_P7
	{ 8, 0x08},		// 0x60  KC_P8
	{ 7, 0x08},		// 0x61  KC_P9
	{ 8, 0x80},		// 0x62  KC_P0
	{ 7, 0x80},		// 0x63  KC_PDOT
	{ 4, 0x08},		// 0x64  KC_NUBS
	{ 6, 0x04},		// 0x65  KC_APP
	{ 0, 0x00},		// 0x66
	{ 0, 0x00},		// 0x67
	{ 0, 0x00},		// 0x68
	{ 0, 0x00},		// 0x69
	{ 0, 0x00},		// 0x6a
	{ 0, 0x00},		// 0x6b
	{ 0, 0x00},		// 0x6c
	{ 0, 0x00},		// 0x6d
	{ 0, 0x00},		// 0x6e
	{ 0, 0x00},		// 0x6f
	{ 0, 0x00},		// 0x70
	{ 0, 0x00},		// 0x71
	{ 0, 0x00},		// 0x72
	{ 0, 0x00},		// 0x73
	{ 0, 0x00},		// 0x74
	{ 0, 0x00},		// 0x75
	{ 0, 0x00},		// 0x76
	{ 0, 0x00},		// 0x77
	{ 0, 0x00},		// 0x78
	{ 0, 0x00},		// 0x79
	{ 0, 0x00},		// 0x7a
	{ 0, 0x00},		// 0x7b
	{ 0, 0x00},		// 0x7c
	{ 0, 0x00},		// 0x7d
	{ 0, 0x00},		// 0x7e
	{ 0, 0x00},		// 0x7f
	{ 0, 0x00},		// 0x80
	{ 0, 0x00},		// 0x81
	{ 0, 0x00},		// 0x82
	{ 0, 0x00},		// 0x83
	{ 0, 0x00},		// 0x84
	{ 0, 0x00},		// 0x85
	{ 0, 0x00},		// 0x86
	{ 0, 0x00},		// 0x87
	{ 0, 0x00},		// 0x88
	{ 0, 0x00},		// 0x89
	{ 0, 0x00},		// 0x8a
	{ 0, 0x00},		// 0x8b
	{ 0, 0x00},		// 0x8c
	{ 0, 0x00},		// 0x8d
	{ 0, 0x00},		// 0x8e
	{ 0, 0x00},		// 0x8f
	{ 0, 0x00},		// 0x90
	{ 0, 0x00},		// 0x91
	{ 0, 0x00},		// 0x92
	{ 0, 0x00},		// 0x93
	{ 0, 0x00},		// 0x94
	{ 0, 0x00},		// 0x95
	{ 0, 0x00},		// 0x96
	{ 0, 0x00},		// 0x97
	{ 0, 0x00},		// 0x98
	{ 0, 0x00},		// 0x99
	{ 0, 0x00},		// 0x9a
	{ 0, 0x00},		// 0x9b
	{ 0, 0x00},		// 0x9c
	{ 0, 0x00},		// 0x9d
	{ 0, 0x00},		// 0x9e
	{ 0, 0x00},		// 0x9f
	{ 0, 0x00},		// 0xa0
	{ 0, 0x00},		// 0xa1
	{ 0, 0x00},		// 0xa2
	{ 0, 0x00},		// 0xa3
	{ 0, 0x00},		// 0xa4
	{ 0, 0x00},		// 0xa5
	{ 0, 0x00},		// 0xa6
	{ 0, 0x00},		// 0xa7
	{ 0, 0x00},		// 0xa8
	{ 0, 0x00},		// 0xa9
	{ 0, 0x00},		// 0xaa
	{ 0, 0x00},		// 0xab
	{ 0, 0x00},		// 0xac
	{ 0, 0x00},		// 0xad
	{ 0, 0x00},		// 0xae
	{ 0, 0x00},		// 0xaf
	{ 0, 0x00},		// 0xb0
	{ 0, 0x00},		// 0xb1
	{ 0, 0x00},		// 0xb2
	{ 0, 0x00},		// 0xb3
	{ 0, 0x00},		// 0xb4
	{ 0, 0x00},		// 0xb5
	{ 0, 0x00},		// 0xb6
	{ 0, 0x00},		// 0xb7
	{ 0, 0x00},		// 0xb8
	{ 0, 0x00},		// 0xb9
	{ 0, 0x00},		// 0xba
	{ 0, 0x00},		// 0xbb
	{ 0, 0x00},		// 0xbc
	{ 0, 0x00},		// 0xbd
	{ 0, 0x00},		// 0xbe
	{ 0, 0x00},		// 0xbf
	{ 4, 0x02},		// 0xc0	KC_FMNU
	{ 0, 0x00},		// 0xc1
	{ 0, 0x00},		// 0xc2
	{ 0, 0x00},		// 0xc3
	{ 0, 0x00},		// 0xc4
	{ 0, 0x00},		// 0xc5
	{ 0, 0x00},		// 0xc6
	{ 0, 0x00},		// 0xc7
	{ 0, 0x00},		// 0xc8
	{ 0, 0x00},		// 0xc9
	{ 0, 0x00},		// 0xca
	{ 0, 0x00},		// 0xcb
	{ 0, 0x00},		// 0xcc
	{ 0, 0x00},		// 0xcd
	{ 0, 0x00},		// 0xce
	{ 0, 0x00},		// 0xcf
	{ 0, 0x00},		// 0xd0
	{ 0, 0x00},		// 0xd1
	{ 0, 0x00},		// 0xd2
	{ 0, 0x00},		// 0xd3
	{ 0, 0x00},		// 0xd4
	{ 0, 0x00},		// 0xd5
	{ 0, 0x00},		// 0xd6
	{ 0, 0x00},		// 0xd7
	{ 0, 0x00},		// 0xd8
	{ 0, 0x00},		// 0xd9
	{ 0, 0x00},		// 0xda
	{ 0, 0x00},		// 0xdb
	{ 0, 0x00},		// 0xdc
	{ 0, 0x00},		// 0xdd
	{ 0, 0x00},		// 0xde
	{ 0, 0x00},		// 0xdf
	{ 1, 0x08},		// 0xe0  KC_LCTL
	{ 2, 0x08},		// 0xe1  KC_LSFT
	{ 0, 0x40},		// 0xe2  KC_LALT
	{ 5, 0x01},		// 0xe3  KC_LGUI
	{ 1, 0x80},		// 0xe4  KC_RCTL
	{ 2, 0x80},		// 0xe5  KC_RSFT
	{ 0, 0x20},		// 0xe6  KC_RALT
	{ 4, 0x02},		// 0xe7  KC_RGUI
};

bool is_pressed_keycode(uint8_t keycode)
{
	uint8_t row, mask;
	row = keycode2matrix[keycode].row;
	mask = keycode2matrix[keycode].mask;

	return matrix[row] & mask;
}

uint8_t get_num_keys_pressed(void)
{
	return matrix_num_keys_pressed;
}