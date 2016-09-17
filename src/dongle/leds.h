#ifndef LEDS_H
#define LEDS_H

#ifdef AVR
#	define LED_on()			SetBit(PORT(LED1_PORT), LED1_BIT)
#	define LED_off()		ClrBit(PORT(LED1_PORT), LED1_BIT)
#else
#	define LED_on()			P03 = 1
#	define LED_off()		P03 = 0
#endif // AVR


#endif	// LEDS_H