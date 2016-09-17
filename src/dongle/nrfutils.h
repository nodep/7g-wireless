#ifndef NRFUTILS_H
#define NRFUTILS_H

#include <stdint.h>

#ifndef __SDCC_REVISION
#define	__idata idata
#define	__xdata xdata
#define	__code code
#define	__reentrant reentrant
#endif

void delay_us(uint16_t __idata us);
void delay_ms(uint16_t __idata ms);

#define TogP(p)		p = p ? 0 : 1

#define _BV(b)		(1 << (b))

#endif	// NRFUTILS_H