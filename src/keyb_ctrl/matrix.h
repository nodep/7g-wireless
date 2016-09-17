#pragma once

// the keyboard matric dimensions
#define	NUM_ROWS	16
#define	NUM_COLS	8

// the state keyboard matrix bit map
extern uint8_t matrix[NUM_ROWS];

void matrix_init(void);
bool matrix_scan(void);

// returns the keycode of the key at a position on the matrix
uint8_t get_keycode(uint8_t row, uint8_t col);

// checks if the key with the given keycode is pressed
bool is_pressed_keycode(uint8_t keycode);

// checks if the key at the given position in the matrix is pressed
#define is_pressed_matrix(row, col)		(matrix[row] & _BV(col))

// reurnes the number of keys that were pressed during to the last matrix scan
uint8_t get_num_keys_pressed(void);