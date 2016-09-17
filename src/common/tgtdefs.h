#ifndef TGTDEFS_H
#define TGTDEFS_H

#include <stddef.h>

#ifdef AVR
#	include <avr/io.h>
#	include <avr/pgmspace.h>
#	include <util/delay.h>
#	define __FLASH_ATTR __flash
#	define __xdata
#	define __code
#	define memcpy_X(dst, src, cnt)	memcpy(dst, src, cnt)
#else
#	ifdef NRF24LE1
#		include <reg24le1.h>
#	elif defined(NRF24LU1)
#		include "reg24lu1.h"
#	else
#		error "Target unsupported!"
#	endif
#	define __FLASH_ATTR		__code
#	define _BV(b)			(1 << (b))

void* memcpy_P(__xdata void* dest, __code const void* src, size_t count);
__xdata void* memcpy_X(__xdata void* dest, __xdata const void* src, size_t count);

#endif

#endif  // TGTDEFS_H