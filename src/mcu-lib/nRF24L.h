#ifndef NRF24L_H
#define NRF24L_H

// definitions for selecting and enabling nRF module
#ifdef AVR
#   include <avr/io.h>
#   include <util/delay.h>
#   include "hw_setup.h"
#   include "avrutils.h"
#   define nRF_CSN_hi()		SetBit(PORT(NRF_CSN_PORT), NRF_CSN_BIT)
#   define nRF_CSN_lo()		ClrBit(PORT(NRF_CSN_PORT), NRF_CSN_BIT)
#   define nRF_CE_hi()		SetBit(PORT(NRF_CE_PORT), NRF_CE_BIT)
#   define nRF_CE_lo()		ClrBit(PORT(NRF_CE_PORT), NRF_CE_BIT)
#else
#   ifdef NRF24LE1
#       include <reg24le1.h>
#   elif defined(NRF24LU1)
#       include <reg24lu1.h>
#   endif
#   define nRF_CSN_hi()		RFCSN = 1
#   define nRF_CSN_lo()		RFCSN = 0
#   define nRF_CE_hi()		RFCE = 1
#   define nRF_CE_lo()		RFCE = 0
#endif

#include "tgtdefs.h"

///////////////////////
// the registers
///////////////////////
enum nRFRegister_e
{
	CONFIG		= 0x00,
	EN_AA		= 0x01,
	EN_RXADDR	= 0x02,
	SETUP_AW	= 0x03,
	SETUP_RETR	= 0x04,
	RF_CH		= 0x05,
	RF_SETUP	= 0x06,
	STATUS		= 0x07,
	OBSERVE_TX	= 0x08,
	RPD			= 0x09,		// CD in nRF24L01
	RX_ADDR_P0	= 0x0A,
	RX_ADDR_P1	= 0x0B,
	RX_ADDR_P2	= 0x0C,
	RX_ADDR_P3	= 0x0D,
	RX_ADDR_P4	= 0x0E,
	RX_ADDR_P5	= 0x0F,
	TX_ADDR		= 0x10,
	RX_PW_P0	= 0x11,
	RX_PW_P1	= 0x12,
	RX_PW_P2	= 0x13,
	RX_PW_P3	= 0x14,
	RX_PW_P4	= 0x15,
	RX_PW_P5	= 0x16,
	FIFO_STATUS	= 0x17,

	DYNPD		= 0x1C,
	FEATURE		= 0x1D,

	undefined_reg = 0xff,
};

///////////////////////
// register bits
///////////////////////

// CONFIG
#define MASK_RX_DR	6
#define MASK_TX_DS	5
#define MASK_MAX_RT	4
#define EN_CRC		3
#define CRCO		2
#define PWR_UP		1
#define PRIM_RX		0

// EN_AA
#define ENAA_P5		5
#define ENAA_P4		4
#define ENAA_P3		3
#define ENAA_P2		2
#define ENAA_P1		1
#define ENAA_P0		0

// EN_RXADDR
#define ERX_P5		5
#define ERX_P4		4
#define ERX_P3		3
#define ERX_P2		2
#define ERX_P1		1
#define ERX_P0		0

// SETUP_AW - not needed
// SETUP_RETR - 
// RF_CH - not needed

// RF_SETUP
#define CONT_WAVE	7
#define RF_DR_LOW	5
#define PLL_LOCK	4
#define RF_DR_HIGH	3
#define RF_PWR_HIGH	2
#define RF_PWR_LOW	1
#define LNA_HCURR	0	// obsolete in nRF24L01+

// STATUS
#define RX_DR		6
#define TX_DS		5
#define MAX_RT		4
#define STS_TX_FULL	0		// renamed to avoid collision with FIFO_STATUS TX_FULL

// OBSERVE_TX - values defined below

// RPD - CD in nRF24L01
#define RPD_BIT		0

// RX_ADDR_Px - not needed
// TX_ADDR - not needed
// RX_PW_Px - not needed

// FIFO_STATUS
#define TX_REUSE		6
#define FIFO_TX_FULL	5	// renamed to avoid collision with STATUS TX_FULL
#define TX_EMPTY		4
#define RX_FULL			1
#define RX_EMPTY		0

// DYNPD
#define DPL_P5			5
#define DPL_P4			4
#define DPL_P3			3
#define DPL_P2			2
#define DPL_P1			1
#define DPL_P0			0

// FEATURE
#define EN_DPL			2
#define EN_ACK_PAY		1
#define EN_DYN_ACK		0


///////////////////////
// register bit values
///////////////////////

// CONFIG
#define vMASK_RX_DR		(1 << MASK_RX_DR)
#define vMASK_TX_DS		(1 << MASK_TX_DS)
#define vMASK_MAX_RT	(1 << MASK_MAX_RT)
#define vEN_CRC			(1 << EN_CRC)
#define vCRCO			(1 << CRCO)
#define vPWR_UP			(1 << PWR_UP)
#define vPRIM_RX		(1 << PRIM_RX)

// EN_AA
#define vENAA_P5		(1 << ENAA_P5)
#define vENAA_P4		(1 << ENAA_P4)
#define vENAA_P3		(1 << ENAA_P3)
#define vENAA_P2		(1 << ENAA_P2)
#define vENAA_P1		(1 << ENAA_P1)
#define vENAA_P0		(1 << ENAA_P0)

// EN_RXADDR
#define vERX_P5			(1 << ERX_P5)
#define vERX_P4			(1 << ERX_P4)
#define vERX_P3			(1 << ERX_P3)
#define vERX_P2			(1 << ERX_P2)
#define vERX_P1			(1 << ERX_P1)
#define vERX_P0			(1 << ERX_P0)

// SETUP_AW -- address widths in bytes
#define vAW_3			1
#define vAW_4			2
#define vAW_5			3

// RF_SETUP
#define vCONT_WAVE		(1 << CONT_WAVE)
#define vRF_DR_LOW		(1 << RF_DR_LOW)
#define vPLL_LOCK		(1 << PLL_LOCK)
#define vRF_DR_HIGH		(1 << RF_DR_HIGH)
#define vRF_PWR_HIGH	(1 << RF_PWR_HIGH)
#define vRF_PWR_LOW		(1 << RF_PWR_LOW)
#define vLNA_HCURR		(1 << LNA_HCURR)	// obsolete in nRF24L01+

#define vRF_DR_1MBPS	0
#define vRF_DR_2MBPS	(1 << RF_DR_HIGH)
#define vRF_DR_250KBPS	(1 << RF_DR_LOW)

#define vRF_PWR_M18DBM	0   // 0b000
#define vRF_PWR_M12DBM	2   // 0b010
#define vRF_PWR_M6DBM	4   // 0b100
#define vRF_PWR_0DBM	6   // 0b110

#define vARD_250us		0x00
#define vARD_500us		0x10
#define vARD_750us		0x20
#define vARD_1000us		0x30
#define vARD_1250us		0x40
#define vARD_1500us		0x50
#define vARD_1750us		0x60
#define vARD_2000us		0x70
#define vARD_2250us		0x80
#define vARD_2500us		0x90
#define vARD_2750us		0xA0
#define vARD_3000us		0xB0
#define vARD_3250us		0xC0
#define vARD_3500us		0xD0
#define vARD_3750us		0xE0
#define vARD_4000us		0xF0

// STATUS
#define vRX_DR			(1 << RX_DR)
#define vTX_DS			(1 << TX_DS)
#define vMAX_RT			(1 << MAX_RT)
#define vSTS_TX_FULL	(1 << STS_TX_FULL)

#define RX_P_NO(stat)			((stat >> 1) & 7)
#define HAS_RX_PAYLOAD(stat)	((stat & 14) < 12)      // ((stat & 0b1110) < 0b1100)

// FIFO_STATUS
#define vTX_REUSE		(1 << TX_REUSE)
#define vFIFO_TX_FULL	(1 << FIFO_TX_FULL)
#define vTX_EMPTY		(1 << TX_EMPTY)
#define vRX_FULL		(1 << RX_FULL)
#define vRX_EMPTY		(1 << RX_EMPTY)

// OBSERVE_TX
#define vARC_CNT		0x0F
#define vPLOS_CNT		0xF0

// DYNPD
#define vDPL_P5			(1 << DPL_P5)
#define vDPL_P4			(1 << DPL_P4)
#define vDPL_P3			(1 << DPL_P3)
#define vDPL_P2			(1 << DPL_P2)
#define vDPL_P1			(1 << DPL_P1)
#define vDPL_P0			(1 << DPL_P0)

// FEATURE
#define vEN_DPL			(1 << EN_DPL)
#define vEN_ACK_PAY		(1 << EN_ACK_PAY)
#define vEN_DYN_ACK		(1 << EN_DYN_ACK)



// commands
#define R_REGISTER			0   // 0b00000000
#define W_REGISTER			32	// 0b00100000
#define R_RX_PAYLOAD		97	// 0b01100001
#define W_TX_PAYLOAD		160	// 0b10100000
#define FLUSH_TX			225	// 0b11100001
#define FLUSH_RX			226	// 0b11100010
#define REUSE_TX_PL			227	// 0b11100011 
#define R_RX_PL_WID			96	// 0b01100000
#define W_ACK_PAYLOAD		168	// 0b10101000
#define W_TX_PAYLOAD_NOACK	176	// 0b10110000
#define NOP					255	// 0b11111111

#define MAX_COMMAND_LENGTH	33

// This buffer is used to send and receive data through SPI to the nRF24L01+ chip.
// It is used by the nRF commands to prepare the command to be sent over SPI and also
// receives the data sent by the chip. After each of the command functions
// this buffer will contain the value of STATUS register in nRF_data[0] and
// any data the command read from the chip.
extern __xdata uint8_t nRF_data[MAX_COMMAND_LENGTH];

void nRF_Init(void);

// All the command functions return the value of the STATUS register.
// The nRF_Read* commands store the data from the chip starting from nRF_data[1]

// read/write the single-byte registers
uint8_t nRF_WriteReg(const enum nRFRegister_e reg, const uint8_t val);
uint8_t nRF_ReadReg(const enum nRFRegister_e reg);

// read/write the address registers
uint8_t nRF_WriteAddrReg(const enum nRFRegister_e reg, const uint8_t __code * addr, const uint8_t addr_len);
uint8_t nRF_ReadAddrReg(const enum nRFRegister_e reg, const uint8_t addr_len);

// reads the RX payload (max num_bytes == 32)
uint8_t nRF_ReadRxPayload(const uint8_t num_bytes);

// writes the TX payload (max num_bytes == 32)
uint8_t nRF_WriteTxPayload(const uint8_t* payload, const uint8_t num_bytes);

// tries to send the last payload again (needs CE pulse)
uint8_t nRF_ReuseTxPayload(void);

// the flush commands
uint8_t nRF_FlushTX(void);
uint8_t nRF_FlushRX(void);

// read the payload width on the receiver side
uint8_t nRF_ReadRxPayloadWidth(void);

// writes the ACK payload (num_bytes <= 32, pipe_num <= 5)
uint8_t nRF_WriteAckPayload(const uint8_t __xdata * payload, const uint8_t num_bytes, const uint8_t pipe_num);

// the NOP command
uint8_t nRF_NOP(void);

// This function gets around the fact that nRF does not have a reset feature.
// For this to work we need a voltage regulator with an on/off feature.
// It shuts off the power to the nRF chip, then waits a little for the filtering
// capacitor to discharge, then turns the voltage regulator back on and waits a
// little for the voltage to stabilize and recharge the capacitor.
#ifdef NRF_PWR_PORT
void nRF_Reset(void);
#endif

#endif  // NRF24L_H