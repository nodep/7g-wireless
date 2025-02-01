#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "nRF24L.h"
#include "rf_protocol.h"
#include "rf_ctrl.h"
#include "led.h"
#include "sleeping.h"
#include "ctrl_settings.h"

// plugging in AVR Dragon's ISP cable will cause the nRF module check to fail,
// even if the nRF works without problems.
// the reason is the pull-up on the the dragon's MISO line. oh well...
//#define NRF_CHECK_MODULE

// we want to count the lost packets
uint32_t plos_total, arc_total, rf_packets_total;

#define NRF_CHECK_MODULE

void rf_set_addr(const uint8_t* addr)
{
	// write the addresses
	nRF_WriteAddrReg(TX_ADDR, addr, NRF_ADDR_SIZE);

	// we need to set the RX address to the same as TX to be
	// able to receive ACK
	nRF_WriteAddrReg(RX_ADDR_P0, addr, NRF_ADDR_SIZE);
}

void rf_ctrl_init(void)
{
	nRF_Init();

	rf_set_addr(DongleAddr1);

#ifdef NRF_CHECK_MODULE

	nRF_data[1] = 0;
	nRF_data[2] = 0;
	nRF_data[3] = 0;
	nRF_data[4] = 0;
	nRF_data[5] = 0;

	nRF_ReadAddrReg(TX_ADDR, NRF_ADDR_SIZE);	// read the address back

	// compare
	if (memcmp(nRF_data + 1, &DongleAddr1, NRF_ADDR_SIZE) != 0)
	{
		printf("buff=%02x %02x %02x %02x %02x\n", DongleAddr1[0], DongleAddr1[1], DongleAddr1[2], DongleAddr1[3], DongleAddr1[4]);
		printf("nRF_=%02x %02x %02x %02x %02x\n", nRF_data[1], nRF_data[2], nRF_data[3], nRF_data[4], nRF_data[5]);
		
		// toggle the CAPS LED forever
		for (;;)
		{
			TogBit(PORT(LED_CAPS_PORT), LED_CAPS_BIT);

			_delay_ms(300);
		}
	}

#endif	// NRF_CHECK_MODULE

	nRF_WriteReg(EN_AA, vENAA_P0);			// enable auto acknowledge
	nRF_WriteReg(EN_RXADDR, vERX_P0);		// enable RX address (for ACK)
	
	nRF_WriteReg(SETUP_RETR, vARD_250us 	// auto retransmit delay - ARD
							| 0x0f);		// auto retransmit count - ARC
	nRF_WriteReg(FEATURE, vEN_DPL | vEN_ACK_PAY);	// enable dynamic payload length and ACK payload
	nRF_WriteReg(DYNPD, vDPL_P0);					// enable dynamic payload length for pipe 0

	nRF_FlushRX();
	nRF_FlushTX();
	
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags
	nRF_WriteReg(RF_CH, CHANNEL_NUM);					// set the channel
	
	// reset the the lost packet counters
	plos_total = arc_total = rf_packets_total = 0;
}

bool rf_ctrl_send_message(const void* buff, const uint8_t num_bytes)
{
	nRF_WriteReg(RF_SETUP, vRF_DR_2MBPS			// data rate 
							| get_nrf_output_power());	// output power

	nRF_FlushTX();
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO | vPWR_UP);	// power up
	nRF_WriteReg(STATUS, vTX_DS | vRX_DR | vMAX_RT);	// reset the status flag
	nRF_WriteTxPayload(buff, num_bytes);
	
	bool is_sent;

	uint8_t attempts = 0;
	const uint8_t MAX_ATTEMPTS = 45;

	uint8_t ticks = 15;
	const uint8_t TICKS_INCREMENT = 20;
	
	do {
		nRF_CE_hi();	// signal the transceiver to send the packet

		// wait for the nRF to signal an event
		sleep_ticks(3);
		while (PIN(NRF_IRQ_PORT) & _BV(NRF_IRQ_BIT))
			sleep_ticks(1);

		nRF_CE_lo();

		const uint8_t status = nRF_NOP();	// read the status reg
		is_sent = (status & vTX_DS) != 0;	// did we get an ACK?

		nRF_WriteReg(STATUS, vMAX_RT | vTX_DS | vRX_DR);	// reset the status flags
		
		// read the ARC
		nRF_ReadReg(OBSERVE_TX);
		arc_total += nRF_data[1] & 0x0f;
		
		++rf_packets_total;
		
		if (!is_sent)
		{
			++plos_total;
			nRF_ReuseTxPayload();		// send the last message again
			
			if (ticks >= 0xfe - TICKS_INCREMENT)
			{
				sleep_max(5);		// 63ms*5 == 0.315sec
			} else {
				sleep_ticks(ticks);
				ticks += TICKS_INCREMENT;
			}
		}

		++attempts;

	} while (!is_sent  &&  attempts < MAX_ATTEMPTS);

	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO);		// nRF power down
	
	return is_sent;
}

uint8_t rf_ctrl_read_ack_payload(void* buff, const uint8_t buff_size)
{
	uint8_t ret_val = 0;

	nRF_ReadReg(FIFO_STATUS);
	uint8_t fifo_status = nRF_data[1];

	if ((fifo_status & vRX_EMPTY) == 0)
	{
		nRF_ReadRxPayloadWidth();
		uint8_t ack_bytes = nRF_data[1];

		// the max ACK payload size has to be 2
		if (ack_bytes <= 32)
		{
			// read the entire payload
			nRF_ReadRxPayload(ack_bytes);

			// copy up to buff_size bytes
			ret_val = ack_bytes < buff_size ? ack_bytes : buff_size;
			memcpy(buff, nRF_data + 1, ret_val);
		} else {
			nRF_FlushRX();
		}
	}

	return ret_val;
}

void rf_ctrl_get_observe(uint8_t* arc, uint8_t* plos)
{
	nRF_ReadReg(OBSERVE_TX);
	if (arc)
		*arc = nRF_data[1] & 0x0f;

	if (plos)
		*plos = nRF_data[1] >> 4;
}

bool rf_ctrl_process_ack_payloads(uint8_t* msg_buff_free, uint8_t* msg_buff_capacity)
{
	// set defaults
	if (msg_buff_free)		*msg_buff_free = 0;
	if (msg_buff_capacity)	*msg_buff_capacity = 0;

	bool ret_val = false;
	uint8_t buff[3];
	while (rf_ctrl_read_ack_payload(buff, sizeof buff))
	{
		if (buff[0] == MT_LED_STATUS)
		{
			set_leds(buff[1], 25);

		} else if (buff[0] == MT_TEXT_BUFF_FREE) {
			
			ret_val = true;
			
			// make a proper message pointer
			rf_msg_text_buff_state_t* msg_free_buff = (rf_msg_text_buff_state_t*) buff;
			
			if (msg_buff_free)
				*msg_buff_free = msg_free_buff->bytes_free;
				
			if (msg_buff_capacity)
				*msg_buff_capacity = msg_free_buff->bytes_capacity;
		}
	}
	
	return ret_val;
}
