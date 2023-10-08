/*
 * output_control.c
 *
 *  Created on: 21 wrz 2023
 *      Author: grzesiek
 */

#include "output_control.h"

#define HYSTERESIS	10

uint16_t EEMEM setTemperatureEEMEM = 0;
uint8_t EEMEM controlModeEEMEM = MODE_HEATING;

void configureTemperature(int16_t temperature)
{
	eeprom_update_word(&setTemperatureEEMEM, temperature);
}

void configureMode(uint8_t mode)
{
	eeprom_update_byte(&controlModeEEMEM, mode);
}

void temperatureNotification(int16_t temperature)
{
	if(temperature < -2000)
	{
		OUTPUT_OFF();
		return;
	}
	int16_t setTemperature = eeprom_read_word(&setTemperatureEEMEM);
	uint8_t mode = eeprom_read_byte(&controlModeEEMEM);

	if(MODE_HEATING == mode)
	{
		if(temperature >= setTemperature)
		{
			OUTPUT_OFF();
		}
		else if(temperature < (setTemperature - HYSTERESIS))
		{
			OUTPUT_ON();
		}
	}
	else if(MODE_COOLING == mode)
	{
		if(temperature <= setTemperature)
		{
			OUTPUT_OFF();
		}
		else if(temperature > (setTemperature + HYSTERESIS))
		{
			OUTPUT_ON();
		}
	}
}
