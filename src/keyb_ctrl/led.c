#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw_setup.h"
#include "led.h"
#include "avrutils.h"
#include "ctrl_settings.h"

#define USER_BRIGHTNESS		0xff

volatile uint8_t curr_led_status = 0;
volatile uint8_t cycle_counter;		// duration the LEDs are on
const __flash led_sequence_t* sequence = 0;

void turn_on_leds(void)
{
	// turn the needed LEDs on by setting
	// the DDR to output and driving the pin(s) low
	DDRG = curr_led_status;
	PORTG = ~curr_led_status;
	
	// change the PWM duty cycle?
	if (sequence)
		OCR0A += sequence->pwm_delta;
}

void turn_off_leds(void)
{
	// turn all the LEDs off
	DDRG = 0x00;	// all inputs
	PORTG = 0xff;	// all pullups
}

// Timer0 interrupts are used for LED brightness with software PWM
ISR(TIMER0_OVF_vect)
{
	turn_on_leds();
}

ISR(TIMER0_COMP_vect)
{
	turn_off_leds();
	
	if (--cycle_counter == 0)
	{
		if (sequence == 0)
		{
			// stop the timer
			TCCR0A = 0;
		} else {
			++sequence;
			
			uint8_t num_cycles = sequence->num_cycles;
			
			if (num_cycles == 0)
			{
				// stop the timer
				TCCR0A = 0;
				
				// stop the sequence iterator
				sequence = 0;
			} else {
				curr_led_status = sequence->led_status & 0x07;
				cycle_counter = num_cycles;
				OCR0A = sequence->brightness == USER_BRIGHTNESS ? get_led_brightness() : sequence->brightness;
			}
		}
	}
}

void init_leds(void)
{
	// timer interrupts for compare and overflow
	TIMSK0 = _B1(OCIE0A) | _B1(TOIE0);
}

void start_led_timer(void)
{
	// start the timer
	TCCR0A = _BV(CS01) | _BV(CS00);		// normal mode, prescaler 64
}

void set_leds(uint8_t new_led_status, uint8_t num_cycles)
{
	// remember the status
	curr_led_status = new_led_status;

	// if there was a LED sequence stop it
	sequence = 0;

	// init the cycle counter
	cycle_counter = num_cycles;

	// reset the PWM duty cycle
	OCR0A = get_led_brightness();
	
	// is the timer off?
	if (TCCR0A == 0)
	{
		// reset the counter
		TCNT0 = 0;

		turn_on_leds();
		
		start_led_timer();
	}
}

bool are_leds_on(void)
{
	return TCCR0A != 0;
}

void start_led_sequence(const __flash led_sequence_t* seq)
{
	sequence = seq;
	
	// init the PWM duty cycle
	OCR0A = sequence->brightness == USER_BRIGHTNESS ? get_led_brightness() : sequence->brightness;

	// set the status
	curr_led_status = seq->led_status;

	// init the cycle counter
	cycle_counter = seq->num_cycles;

	// is the timer off?
	if (TCCR0A == 0)
	{
		// reset the counter
		TCNT0 = 0;

		turn_on_leds();

		start_led_timer();
	}
}



// LED sequences
const __flash led_sequence_t led_seq_boot[] =
{
	{1, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{2, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{1, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{2, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{1, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{2, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{1, 5, USER_BRIGHTNESS, 0},
	
	{0,0,0,0},
};

const __flash led_sequence_t led_seq_menu_begin[] = 
{
	{1, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{2, 5, USER_BRIGHTNESS, 0},
	
	{0,0,0,0},
};

const __flash led_sequence_t led_seq_menu_end[] = 
{
	{2, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	{1, 5, USER_BRIGHTNESS, 0},
	
	{0,0,0,0},
};

const __flash led_sequence_t led_seq_pulse_on[] = 
{
	{7, 45, 0, 3},
	
	{0,0,0,0},
};

const __flash led_sequence_t led_seq_pulse_off[] = 
{
	{7, 45, 136, -3},

	{0,0,0,0},
};

const __flash led_sequence_t led_seq_lock[] =
{
	{3, 5, USER_BRIGHTNESS, 0},
	{4, 5, USER_BRIGHTNESS, 0},
	
	{0,0,0,0},
};


// function key to brightness lookup
const uint8_t __flash led_brightness_lookup[12] = 
{
	1,		// F1
	3,		// F2
	5,		// F3
	8,		// F4
	13,		// F5
	20,		// F6
	30,		// F7
	50,		// F8
	80,		// F9
	120,	// F10
	190,	// F11
	254,	// F12
};
