#include <stdbool.h>
#include <stdio.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "sleeping.h"
#include "matrix.h"
#include "led.h"
#include "avrutils.h"
#include "avrdbg.h"

// this is our watch
// not 100% accurate, but serves the purpose
// it's updated only from add_ticks() and read from get_seconds() and get_seconds32()
struct 
{
	uint16_t tcnt2_lword;	// overflows every 62.5ms * 256 == 16s
							// resolution == 244.140625us

	uint16_t tcnt2_hword;	// overflows every 1048576s, 291.27111h, 12.13629 days
							// resolution == 16s
							
	uint8_t tcnt2_vhbyte;	// overflows every 3106,89185 days
							// resolution == 12.13629 days
} watch = {0, 0, 0};

uint32_t get_seconds32(void)
{
	uint32_t ret_val = watch.tcnt2_vhbyte;
	ret_val <<= 16;
	ret_val |= watch.tcnt2_hword;
	ret_val <<= 4;
	ret_val |= (watch.tcnt2_lword >> 12);

	return ret_val;
}

uint16_t get_seconds(void)
{
	uint16_t ret_val = watch.tcnt2_hword;
	ret_val <<= 4;
	ret_val |= (watch.tcnt2_lword >> 12);

	return ret_val;
}

void get_time(uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
{
	uint32_t sec = get_seconds32();
	*seconds = sec % 60;
	sec /= 60;
	*minutes = sec % 60;
	sec /= 60;
	*hours = sec % 24;
	sec /= 24;
	*days = sec;
}

void init_sleep(void)
{
	// the AVR draws about 6uA in power save mode
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);

	// config the wake-up timer; the timer is set to normal mode
	TCCR2A = 	//_BV(CS20);				// no prescaler
											// TCNT=30.517578125us  OVF=7.8125ms

				_BV(CS21);					// 8 prescaler
											// TCNT=244.140625us    OVF=62.5ms

				//_BV(CS21) | _BV(CS20);	// 32 prescaler
											// TCNT=976.5625us      OVF=250ms

				//_BV(CS22);				// 64 prescaler
											// TCNT=1953.125us      OVF=500ms

				//_BV(CS22) | _BV(CS20);	// 128 prescaler
											// TCNT=3906.25us       OVF=1000ms

				//_BV(CS22) | _BV(CS21);	// 256 prescaler
											// TCNT=7812.5us        OVF=2000ms

				//_BV(CS22) | _BV(CS21) | _BV(CS20);	// 1024 prescaler
														// TCNT=31250us         OVF=8000ms
														
	// we are running Timer2 from the 32kHz crystal - set to async mode
	ASSR = _BV(AS2);

	TIMSK2 = _BV(TOIE2);	// interrupt on overflow
}

void add_ticks(uint16_t ticks)
{
	// we assume the error on average is half a tick
	// so add a tick every other call to account for this
	static uint8_t correction = 0;
	ticks += correction;
	correction = correction ? 0 : 1;

	// mind the overflow
	if (ticks > 0xffff - watch.tcnt2_lword)
	{
		if (watch.tcnt2_hword == 0xffff)
			++watch.tcnt2_vhbyte;

		++watch.tcnt2_hword;
	}
	
	watch.tcnt2_lword += ticks;
}

// Timer2 overflow interrupt wakes us from sleep
ISR(TIMER2_OVF_vect)
{}

// sleep for sleep_ticks number of TCNT2 ticks
void sleep_ticks(uint8_t ticks)
{
	if (are_leds_on())
	{
		uint8_t prev_ticks = TCNT2;
		
		while (ticks--)
			_delay_us(244.14);
			
		add_ticks(ticks - prev_ticks);
	} else {
		sleep_enable();
		add_ticks(TCNT2);
		
		TCNT2 = 0xff - ticks;					// set the sleep duration
		loop_until_bit_is_clear(ASSR, TCN2UB);	// wait for the update
		
		add_ticks(ticks);
		sleep_mode();				// go to sleep
		sleep_disable();
	}
}

const __flash sleep_schedule_period_t sleep_schedule_default[] =
{
	{   300,   24},		// 5 minutes, ~6ms refresh
	{   900,   33},		// 15 minutes, ~8ms refresh
	{  1800,   82},		// 30 minutes, ~20ms refresh
	{0xffff,  250},		// forever, ~62ms refresh
};

const __flash sleep_schedule_period_t* active_sleep_schedule = sleep_schedule_default;
const __flash sleep_schedule_period_t* curr_sleep_period;
uint16_t sleep_period_started = 0;

void sleep_dynamic(void)
{
	// if the current period is not forever
	if (curr_sleep_period->duration_sec != 0xffff)
	{
		// get the time elapsed in this period
		uint16_t curr_seconds = get_seconds();
		uint16_t sec_elapsed = curr_seconds - sleep_period_started;
		if (sec_elapsed >= curr_sleep_period->duration_sec)
		{
			// advance to the next period
			++curr_sleep_period;
			
			// remember the time
			sleep_period_started = curr_seconds;
		}
	}

	sleep_ticks(curr_sleep_period->num_ticks);
}

// sleep for the entire sleep period a given number of times
void sleep_max(uint8_t num_times)
{
	while (num_times--)
		sleep_ticks(0xfe);
}

void sleep_reset(void)
{
	curr_sleep_period = active_sleep_schedule;
	sleep_period_started = get_seconds();
}

void wait_for_all_keys_up(void)
{
	sleep_reset();
	matrix_scan();
	while (get_num_keys_pressed())
	{
		sleep_dynamic();
		matrix_scan();
	}
}

void wait_for_key_down(void)
{
	sleep_reset();
	matrix_scan();
	while (get_num_keys_pressed() == 0)
	{
		sleep_dynamic();
		matrix_scan();
	}
}

void wait_for_matrix_change(void)
{
	sleep_reset();
	while (!matrix_scan())
		sleep_dynamic();
}
