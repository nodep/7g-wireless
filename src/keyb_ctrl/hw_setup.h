#pragma once

// this file contains the definitions of
// all the uC pins used by the project

//
// nRF (SPI) module pins
//

#define NRF_IRQ_PORT	E
#define NRF_IRQ_BIT		6

#define NRF_CE_PORT		B
#define NRF_CE_BIT		0

#define NRF_CSN_PORT	E
#define NRF_CSN_BIT		5

#define NRF_MOSI_PORT	B
#define NRF_MOSI_BIT	2

#define NRF_MISO_PORT	B
#define NRF_MISO_BIT	3

#define NRF_SCK_PORT	B
#define NRF_SCK_BIT		1


// the LEDs

#define LED_NUML_PORT	G
#define LED_NUML_BIT	0

#define LED_SCRL_PORT	G
#define LED_SCRL_BIT	1

#define LED_CAPS_PORT	G
#define LED_CAPS_BIT	2


// the keyboard matrix uses every pin of ports A, C and D
// Ports A and D are configured as outputs and port C is input
