#ifndef __SDCC_REVISION
#include <intrins.h>
#endif

#include "nrfutils.h"

void delay_us(uint16_t __idata us)
{
	us;		// silence the compiler
	
	__asm

	mov	r6,dpl
	mov	r7,dph
	
delay_us_00101$:
	nop
	nop
	nop
	nop
	nop
	
	dec	r6
	cjne	r6,#0xFF,delay_us_00110$
	dec	r7
delay_us_00110$:
	mov	a,r6
	orl	a,r7
	jnz	delay_us_00101$

	__endasm;
}

void delay_ms(uint16_t __idata ms)
{
	do {
		delay_us(1000);
	} while (--ms);
}
