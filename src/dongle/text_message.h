#pragma once

// contains a ring buffer implementation for the text messages
uint8_t msg_size(void);
uint8_t msg_free(void);
uint8_t msg_capacity(void);
void msg_push(char c);
char msg_pop(void);
char msg_peek(void);
bool msg_full(void);
bool msg_empty(void);

uint8_t get_keycode_for_char(char c);
uint8_t get_modifiers_for_char(char c);
