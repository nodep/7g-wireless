#include <stdint.h>
#include <stdbool.h>

#ifdef NRF24LU1
# include <reg24lu1.h>
#elif defined(NRF24LE1)
# include <reg24le1.h>
#else
# error "Target unsupported."
#endif

#include "nrfdbg.h"
#include "nrfutils.h"

#define BAUD_57K6   1015  // = Round(1024 - (2*16e6)/(64*57600))
#define BAUD_38K4   1011  // = Round(1024 - (2*16e6)/(64*38400))
#define BAUD_19K2    998  // = Round(1024 - (2*16e6)/(64*19200))
#define BAUD_9K6     972  // = Round(1024 - (2*16e6)/(64*9600))

#if defined(DBG_MODE)  &&  DBG_MODE > 0

void dbgInit(void)
{
	uint16_t temp;

	ES0 = 0;			// Disable UART0 interrupt while initializing
	//REN0 = 1;			// Enable receiver
	SM0 = 0;			// Mode 1..
	SM1 = 1;			// ..8 bit, variable baud rate
	PCON |= 0x80; 		// SMOD = 1
#ifdef NRF24LU1
	WDCON |= 0x80;		// Select internal baud rate generator
#else
	ADCON |= 0x80;		// Select internal baud rate generator
#endif
	temp = BAUD_57K6;
	S0RELL = (uint8_t)temp;
	S0RELH = (uint8_t)(temp >> 8);

#ifdef NRF24LU1
	P0ALT |= 0x04;		// Select alternate functions on P02
	P0EXP &= 0xfc;		// Select TxD on P02
#else
	P0DIR &= ~_BV(3);	// P03 is output (only for 32 pin version)
#endif	

	TI0 = 1;
}

void putchar(char c) __reentrant
{
	if (c == '\n')
		putchar('\r');

#if DBG_MODE == 2
	while (dbgFull())
		dbgPoll();

	dbgPush(c);
#else
	if (TI0)
	{
		TI0 = 0;
		S0BUF = c;
		while (TI0 == 0)
			;
	}
#endif
}

#endif

#if DBG_MODE == 2

// this can not be more than 254
#define DBG_MSG_BUFF_SIZE		250

__xdata char dbg_msg_buff[DBG_MSG_BUFF_SIZE];
uint8_t dbg_buff_head = 0;
uint8_t dbg_buff_tail = 0;

void dbgPoll(void)
{
	if (TI0  &&  !dbgEmpty())
	{
		while (TI0 == 0)
			;
		TI0 = 0;
		S0BUF = dbgPop();
	}
}

uint8_t dbgCapacity(void)
{
	return DBG_MSG_BUFF_SIZE - 1;
}

uint8_t dbgSize(void)
{
	if (dbg_buff_head < dbg_buff_tail)
		return DBG_MSG_BUFF_SIZE + dbg_buff_head - dbg_buff_tail;
	
	return dbg_buff_head - dbg_buff_tail;
}

uint8_t dbgFree(void)
{
	return DBG_MSG_BUFF_SIZE - dbgSize() - 1;
}

void dbgPush(char c)
{
	dbg_msg_buff[dbg_buff_head] = c;
	dbg_buff_head = (dbg_buff_head + 1) % DBG_MSG_BUFF_SIZE;
}

void dbgPushInt(uint8_t i)
{
	if (i > 99)
	{
		dbgPush('0' + (i / 100));
		i %= 100;
	}
	
	if (i > 9)
	{
		dbgPush('0' + (i / 10));
		i %= 10;
	}
	
	dbgPush('0' + i);
}

char dbgPop(void)
{
	char ret_val = dbg_msg_buff[dbg_buff_tail];
	dbg_buff_tail = (dbg_buff_tail + 1) % DBG_MSG_BUFF_SIZE;

	return ret_val;
}

char dbgPeek(void)
{
	char ret_val = dbg_msg_buff[dbg_buff_tail];
	return ret_val;
}

bool dbgFull(void)
{
	return ((dbg_buff_head + 1) % DBG_MSG_BUFF_SIZE) == dbg_buff_tail;
}

bool dbgEmpty(void)
{
	return dbg_buff_head == dbg_buff_tail;
}

void dbgFlush(void)
{
	while (!dbgEmpty())
		dbgPoll();
		
	delay_us(200);	// wait until the char is sent
}

void dbgClear(void)
{
	dbg_buff_head = dbg_buff_tail = 0;
}

#endif	// DBG_MODE == 2
