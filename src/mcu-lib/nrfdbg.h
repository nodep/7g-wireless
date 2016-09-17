#ifndef NRFDBG_H
#define NRFDBG_H

#if defined(DBG_MODE)  &&  (DBG_MODE < 0  || DBG_MODE > 2)
# error DBG_MODE is not valid!
#endif

// These functions can be used for a debug output over UART
// for the nRF24LE1 and nRF24LU1+ microncontrollers.

#if defined(DBG_MODE)  &&  DBG_MODE > 0

# ifndef PRINTF
#  define PRINTF			printf_tiny
# endif

void dbgInit(void);
#  define dputs(a)			puts(a)
#  ifndef __C51__
#    define dprintf(...)	PRINTF(__VA_ARGS__)		// gotta love C99, man
#  endif
#  if DBG_MODE == 2
void dbgPush(char c);
void dbgPushInt(uint8_t i);
void dbgPoll(void);
uint8_t dbgCapacity(void);
uint8_t dbgSize(void);
uint8_t dbgFree(void);
char dbgPop(void);
char dbgPeek(void);
bool dbgFull(void);
bool dbgEmpty(void);
void dbgFlush(void);
void dbgClear(void);
#  else		// DBG_MODE == 2
#    define dbgPush(c)		putchar(c)
#    define dbgPoll()
#    define dbgFlush()
#    define dbgEmpty()		1

#  endif

#else
# define dbgInit()
# define dputs(a)
# ifndef __C51__
#  define dprintf(...)
# endif
# define dbgPoll()
# define dbgFlush()
# define dbgEmpty()		1
#endif

// if async mode - we need to call dbgPoll() to send the characters

// Saleae probe colors
#ifdef NRF24LE1
# define BLACK		P05
# define BROWN		P07
# define RED		P10
# define ORANGE		P11
#endif

#endif	// NRFDBG_H
