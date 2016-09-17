#include <stdint.h>

#include <avr/io.h>
#include <avr/cpufunc.h>

#include "calibrate_rc.h"

#define XTAL_FREQUENCY		32768
#define EXTERNAL_TICKS		20		// increase for better accuracy
#define OSCCAL_RESOLUTION	7
#define LOOP_CYCLES			7

// this is a very shortened version of the calibration procedure recommended by Atmel
// we are only checking the upper half of the OSCCAL range: 0x81 to 0xff
void calibrate_rc(void)
{
	uint16_t countVal = ((EXTERNAL_TICKS * F_CPU) / (XTAL_FREQUENCY * LOOP_CYCLES)) * 16;

	// the external timer is already setup - no need to change it

	OSCCAL = OSCCAL_DEFAULT;

	uint8_t cycles;
	for (cycles = 0; cycles < 20; ++cycles)
	{
		uint16_t counter = 0;
		TCNT2 = 0x00;

		while (ASSR & (_BV(OCR2UB)			// output compare update busy
						| _BV(TCN2UB)		// timer update busy
						| _BV(TCR2UB)))		// timer control update busy
			;

		do {								// counter++: Increment counter - the add immediate to word (ADIW) takes 2 cycles of code.
			counter++;						// Devices with async TCNT in I/0 space use 1 cycle reading, 2 for devices with async TCNT in extended I/O space
		} while (TCNT2 < EXTERNAL_TICKS);	// CPI takes 1 cycle, BRCS takes 2 cycles, resulting in: 2+1(or 2)+1+2=6(or 7) CPU cycles

		// did we count too much or too little?
		if (counter > countVal)
		{
			if (OSCCAL == 0x81)
				break;

			OSCCAL--;
			_NOP();
		} else if (counter < countVal) {

			if (OSCCAL == 0xff)
				break;

			OSCCAL++;
			_NOP();
		} else if (counter == countVal) {
			// we're good -- no need to carry on
			break;
		}
	}
}
