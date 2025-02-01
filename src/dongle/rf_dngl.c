#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "leds.h"
#include "rf_protocol.h"
#include "nRF24L.h"

#define NRF_CHECK_MODULE


void rf_dngl_init(void)
{
	nRF_Init();

	// set the addresses
	nRF_WriteAddrReg(RX_ADDR_P0, DongleAddr1, NRF_ADDR_SIZE);

#if defined(NRF_CHECK_MODULE) && defined(AVR)

	nRF_data[1] = 0;
	nRF_data[2] = 0;
	nRF_data[3] = 0;
	nRF_data[4] = 0;
	nRF_data[5] = 0;

	nRF_ReadAddrReg(RX_ADDR_P0, NRF_ADDR_SIZE);	// read the address back
	
	// compare
	if (memcmp(nRF_data + 1, &DongleAddr1, NRF_ADDR_SIZE) != 0)
	{
		//printf("buff=%02x %02x %02x %02x %02x\n", buff[0], buff[1], buff[2], buff[3], buff[4]);
		//printf("nRF_=%02x %02x %02x %02x %02x\n", nRF_data[1], nRF_data[2], nRF_data[3], nRF_data[4], nRF_data[5]);

		// toggle the LED forever
		for (;;)
		{
			TogBit(PORT(LED1_PORT), LED1_BIT);

			_delay_ms(300);
		}
	}

#endif	// NRF_CHECK_MODULE

	nRF_WriteReg(EN_AA, vENAA_P0);			// enable auto acknowledge
	nRF_WriteReg(SETUP_RETR, vARD_250us);	// ARD=250us, ARC=disabled
	nRF_WriteReg(RF_SETUP, vRF_DR_2MBPS		// data rate
						| vRF_PWR_0DBM);	// output power

	nRF_WriteReg(FEATURE, vEN_DPL | vEN_ACK_PAY);	// enable dynamic payload length and ACK payload
	nRF_WriteReg(DYNPD, vDPL_P0);					// enable dynamic payload length for pipe 0

	nRF_FlushRX();
	nRF_FlushTX();
	
	nRF_WriteReg(EN_RXADDR, vERX_P0);					// enable RX address
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags

	nRF_WriteReg(RF_CH, CHANNEL_NUM);			// set the channel
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO 		// enable a 2 byte CRC
								| vMASK_TX_DS	// we don't care about the TX_DS status flag
								| vPRIM_RX		// RX mode
								| vPWR_UP);		// power up the transceiver

	nRF_CE_hi();		// start receiving
}

uint8_t rf_dngl_recv(__xdata void* buff, uint8_t buff_size)
{
	uint8_t ret_val = 0;
	
	// check if there's data in the RX FIFO
	nRF_ReadReg(FIFO_STATUS);
	if ((nRF_data[1] & vRX_EMPTY) == 0)
	{
		LED_on();
		
		// read the payload
		nRF_ReadRxPayloadWidth();
		ret_val = nRF_data[1];

		// the nRF specs state I have to drop the packet if the length is > 32
		if (ret_val > 32)
		{
			nRF_FlushRX();
			ret_val = 0;
		} else {
			nRF_ReadRxPayload(ret_val);
			memcpy_X(buff, nRF_data + 1, ret_val > buff_size ? buff_size : ret_val);
		}

		// reset the TX_DS
		if (nRF_data[0] & vTX_DS)
			nRF_WriteReg(STATUS, vTX_DS);

		LED_off();
	}
	
	return ret_val;
}

void rf_dngl_queue_ack_payload(__xdata void* buff, const uint8_t num_bytes)
{
	// get the TX FIFO status
	nRF_ReadReg(FIFO_STATUS);

	// clear any unsent ACK payloads
	if (!(nRF_data[1]  &  vTX_EMPTY))
		nRF_FlushTX();

	// send the payload
	nRF_WriteAckPayload(buff, num_bytes, 0);	// pipe 0
}
