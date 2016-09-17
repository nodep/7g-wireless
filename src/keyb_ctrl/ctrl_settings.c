#include <stdbool.h>
#include <stdint.h>

#include <avr/eeprom.h>

#include "nRF24L.h"
#include "led.h"
#include "ctrl_settings.h"

#define MIN_LED_BRIGHTNESS			1
#define MAX_LED_BRIGHTNESS			0xfe
#define DEFAULT_LED_BRIGHTNESS		MIN_LED_BRIGHTNESS

uint8_t EEMEM led_brightness;
uint8_t EEMEM nrf_output_power;

uint8_t get_led_brightness(void)
{
	uint8_t ret_val = eeprom_read_byte(&led_brightness);
	if (ret_val > MAX_LED_BRIGHTNESS)	// if not set yet
		ret_val = DEFAULT_LED_BRIGHTNESS;

	return ret_val;
}

uint8_t get_nrf_output_power(void)
{
	uint8_t ret_val = eeprom_read_byte(&nrf_output_power);
	
	// default to highest power
	if (ret_val != vRF_PWR_M18DBM
			&&  ret_val != vRF_PWR_M12DBM
			&&  ret_val != vRF_PWR_M6DBM)
	{
		ret_val = vRF_PWR_0DBM;
	}

	return ret_val;
}

void set_led_brightness(uint8_t new_val)
{
	if (new_val == 0xff)
		new_val = 0xfe;

	eeprom_update_byte(&led_brightness, new_val);
	
	init_leds();
}

void set_nrf_output_power(uint8_t new_val)
{
	if (new_val != vRF_PWR_M18DBM
			&&  new_val != vRF_PWR_M12DBM
			&&  new_val != vRF_PWR_M6DBM)
	{
		new_val = vRF_PWR_0DBM;
	}
	
	eeprom_update_byte(&nrf_output_power, new_val);
}
