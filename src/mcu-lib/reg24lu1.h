#ifndef NRF24LU1_H
#define NRF24LU1_H

#include "compiler_mcs51.h"

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR(P0,		0x80);
SFR(SP,		0x81);
SFR(DPL,	0x82);
SFR(DPH,	0x83);
SFR(DPL1,	0x84);
SFR(DPH1,	0x85);
SFR(PCON,	0x87);
SFR(TCON,	0x88);
SFR(TMOD,	0x89);
SFR(TL0,	0x8A);
SFR(TL1,	0x8B);
SFR(TH0,	0x8C);
SFR(TH1,	0x8D);
SFR(CKCON,	0x8E);
SFR(RFCON,	0x90);
SFR(DPS,	0x92);
SFR(P0DIR,	0x94);
SFR(P0ALT,	0x95);
SFR(S0CON,	0x98);
SFR(S0BUF,	0x99);
SFR(USBCON,	0xA0);
SFR(CLKCTL,	0xA3);
SFR(PWRDWN,	0xA4);
SFR(WUCONF,	0xA5);
SFR(INTEXP,	0xA6);
SFR(IEN0,	0xA8);
SFR(IP0,	0xA9);
SFR(S0RELL,	0xAA);
SFR(REGXH,	0xAB);
SFR(REGXL,	0xAC);
SFR(REGXC,	0xAD);
SFR(RSTRES,	0xB1);
SFR(SMDAT,	0xB2);
SFR(SMCTL,	0xB3);
SFR(TICKDV,	0xB5);
SFR(IEN1,	0xB8);
SFR(IP1,	0xB9);
SFR(S0RELH,	0xBA);
SFR(SSCONF,	0xBC);
SFR(SSDATA,	0xBD);
SFR(SSSTAT,	0xBE);
SFR(IRCON,	0xC0);
SFR(CCEN,	0xC1);
SFR(CCL1,	0xC2);
SFR(CCH1,	0xC3);
SFR(CCL2,	0xC4);
SFR(CCH2,	0xC5);
SFR(CCL3,	0xC6);
SFR(CCH3,	0xC7);
SFR(T2CON,	0xC8);
SFR(P0EXP,	0xC9);
SFR(CRCL,	0xCA);
SFR(CRCH,	0xCB);
SFR(TL2,	0xCC);
SFR(TH2,	0xCD);
SFR(PSW,	0xD0);
SFR(WDCON,	0xD8);
SFR(USBSLP,	0xD9);
SFR(ACC,	0xE0);
SFR(RFDAT,	0xE5);
SFR(RFCTL,	0xE6);
SFR(AESCS,	0xE8);
SFR(MD0,	0xE9);
SFR(MD1,	0xEA);
SFR(MD2,	0xEB);
SFR(MD3,	0xEC);
SFR(MD4,	0xED);
SFR(MD5,	0xEE);
SFR(ARCON,	0xEF);
SFR(B,		0xF0);
SFR(AESKIN,	0xF1);
SFR(AESIV,	0xF2);
SFR(AESD,	0xF3);
SFR(AESIA1,	0xF5);
SFR(AESIA2,	0xF6);
SFR(FSR,	0xF8);
SFR(FPCR,	0xF9);
SFR(FCR,	0xFA);

//-----------------------------------------------------------------------------
// Word Registers
//-----------------------------------------------------------------------------

SFR16(CC1,	0xC2);
SFR16(CC2,	0xC4);
SFR16(CC3,	0xC6);
SFR16(CRC,	0xCA);
SFR16(T2,	0xCC);

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// FSR
SBIT(DBG,		0xF8, 7);
SBIT(STP,		0xF8, 6);
SBIT(WEN,		0xF8, 5);
SBIT(RDYN,		0xF8, 4);
SBIT(INFEN,		0xF8, 3);
SBIT(RDISMB,	0xF8, 2);
SBIT(RDISIP,	0xF8, 1);
SBIT(WPEN,		0xF8, 0);

// PSW
SBIT(CY,	0xD0, 7);
SBIT(AC,    0xD0, 6);
SBIT(F0,    0xD0, 5);
SBIT(RS1,   0xD0, 4);
SBIT(RS0,   0xD0, 3);
SBIT(OV,    0xD0, 2);
SBIT(F1,    0xD0, 1);
SBIT(P,     0xD0, 0);

// TCON
SBIT(TF1,	0x88, 7);
SBIT(TR1,	0x88, 6);
SBIT(TF0,	0x88, 5);
SBIT(TR0,	0x88, 4);
SBIT(IE1,	0x88, 3);
SBIT(IT1,	0x88, 2);
SBIT(IE0,	0x88, 1);
SBIT(IT0,	0x88, 0);

// S0CON
SBIT(SM0,	0x98, 7);
SBIT(SM1,   0x98, 6);
SBIT(SM20,  0x98, 5);
SBIT(REN0,  0x98, 4);
SBIT(TB80,  0x98, 3);
SBIT(RB80,  0x98, 2);
SBIT(TI0,   0x98, 1);
SBIT(RI0,   0x98, 0);

// T2CON 0xC8
SBIT(T2PS,	0xC8, 7);
SBIT(I3FR,	0xC8, 6);
SBIT(I2FR,	0xC8, 5);
SBIT(T2R1,	0xC8, 4);
SBIT(T2R0,	0xC8, 3);
SBIT(T2CM,	0xC8, 2);
SBIT(T2I1,	0xC8, 1);
SBIT(T2I0,	0xC8, 0);

// IEN0
SBIT(EA,	0xA8, 7);

SBIT(ET2,	0xA8, 5);
SBIT(ES0,	0xA8, 4);
SBIT(ET1,	0xA8, 3);
SBIT(EX1,	0xA8, 2);
SBIT(ET0,	0xA8, 1);
SBIT(EX0,	0xA8, 0);

// IEN1
SBIT(EXEN2,		0xB8, 7);

SBIT(WUIRQ,     0xB8, 5);
SBIT(USBIRQEN,  0xB8, 4);
SBIT(USBWUEN,   0xB8, 3);
SBIT(SPI,       0xB8, 2);
SBIT(RF,        0xB8, 1);
SBIT(RFSPI,     0xB8, 0);

// IRCON
SBIT(EXF2,		0xC0, 7);
SBIT(TF2,		0xC0, 6);
SBIT(WUF,		0xC0, 5);
SBIT(USBIRQ,	0xC0, 4);
SBIT(USBWU,		0xC0, 3);
SBIT(SPIF,		0xC0, 2);
SBIT(RFIRQ,		0xC0, 1);
SBIT(RFSPIF,	0xC0, 0);	// RFRDY

// USBCON
SBIT(SWRST,		0xA0, 7);
SBIT(WU,		0xA0, 6);
SBIT(SUSPEND,	0xA0, 5);
SBIT(IV4,		0xA0, 4);
SBIT(IV3,		0xA0, 3);
SBIT(IV2,		0xA0, 2);
SBIT(IV1,		0xA0, 1);
SBIT(IV0,		0xA0, 0);

// PORT0
SBIT(P00,		0x80, 0);
SBIT(P01,		0x80, 1);
SBIT(P02,		0x80, 2);
SBIT(P03,		0x80, 3);
SBIT(MCSN,		0x80, 3);
SBIT(SCSN,		0x80, 3);
SBIT(P04,		0x80, 4);
SBIT(P05,		0x80, 5);

// RFCON
SBIT(RFCE,		0x90, 0);
SBIT(RFCSN,		0x90, 1);
SBIT(RFCKEN,	0x90, 2);

// WDCON
SBIT(BD,		0xD8, 7);

// AESCS 0xE8
SBIT(GO,		0xD8, 0);
SBIT(DECR,		0xD8, 1);

// REGX commands
#define RWD         0x00
#define WWD         0x08
#define RGTIMER     0x01
#define WGTIMER     0x09
#define RRTCLAT     0x02
#define WRTCLAT     0x0A
#define RRTC        0x03
#define WRTCDIS     0x0B
#define RWSTA0      0x04
#define WWCON0      0x0C
#define RWSTA1      0x05
#define WWCON1      0x0D

//-----------------------------------------------------------------------------
// Interrupt Vector Definitions
//-----------------------------------------------------------------------------

#define INTERRUPT_EXT_INT0     0   // External Interrupt0 (P0.3)
#define INTERRUPT_T0           1   // Timer0 Overflow
#define INTERRUPT_AES_RDY      2   // AES ready interrupt
#define INTERRUPT_T1           3   // Timer1 Overflow
#define INTERRUPT_UART0        4   // UART0, Receive & Transmitt interrupt
#define INTERRUPT_T2           5   // Timer2 Overflow
#define INTERRUPT_RF_RDY       8   // RF SPI ready interrupt
#define INTERRUPT_RFIRQ        9   // RF interrupt
#define INTERRUPT_SPI          10  // SPI interrupt
#define INTERRUPT_USB_WU       11  // USB wakeup interrupt
#define INTERRUPT_USB_INT      12  // USB interrupt
#define INTERRUPT_WU           13  // Internal wakeup interrupt

#endif	// NRF24LU1_H