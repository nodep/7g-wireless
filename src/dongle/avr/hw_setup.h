#pragma once

// this file contains the definitions of
// all the uC pins used by the wireless USB dongle

//
// nRF (SPI) module pins
//

#define NRF_IRQ_PORT	D
#define NRF_IRQ_BIT		7

#define NRF_CE_PORT		B
#define NRF_CE_BIT		0

#define NRF_CSN_PORT	B
#define NRF_CSN_BIT		1

#define NRF_MOSI_PORT	B
#define NRF_MOSI_BIT	3

#define NRF_MISO_PORT	B
#define NRF_MISO_BIT	4

#define NRF_SCK_PORT	B
#define NRF_SCK_BIT		5

// this one needs to be driven for master SPI to work
#define NRF_SS_PORT		B
#define NRF_SS_BIT		2


// the LEDs

#define LED1_PORT	C
#define LED1_BIT	0

#define LED2_PORT	C
#define LED2_BIT	1

#define LED3_PORT	C
#define LED3_BIT	2
