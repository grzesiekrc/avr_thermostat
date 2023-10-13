/*
 * display.c
 *
 *  Created on: 20 wrz 2023
 *      Author: grzesiek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"
#include "output_control.h"
#include "utils.h"

extern int16_t temperature;
extern volatile uint8_t timCounter;

uint8_t displayState;
uint8_t displayStateCounter;

int16_t temporarySetTemperature;
int16_t temporaryMode;

volatile uint8_t displayBytes[DISPLAY_DIGITS];
volatile uint8_t currentDigit;

static const uint8_t segment1DigitValues[] =
{
	0x81, // 0
	0xE7, // 1
	0x49, // 2
	0x43, // 3
	0x27, // 4
	0x13, // 5
	0x11, // 6
	0x87, // 7
	0x01, // 8
	0x03, // 9
	0x7F, // -
	0xFE, // DOT
	0xFF, // NOTHING
	0xFB, // _
	0xDF // Upper line
};

static const uint8_t segment2DigitValues[] =
{
	0x12, // 0
	0x7B, // 1
	0x26, // 2
	0x23, // 3
	0x4B, // 4
	0x83, // 5
	0x82, // 6
	0x1B, // 7
	0x02, // 8
	0x03, // 9
	0xEF, // -
	0xFD, // DOT
	0xFF, // NOTHING
	0xF7, // _
	0xBF // Upper line
};

#define DISPLAY_MINUS	10
#define DISPLAY_DOT		11
#define DISPLAY_NOTHING	12
#define DISPLAY_UNDERSCORE 13
#define DISPLAY_UPPERLINE 14

ISR(SPI_STC_vect)
{
	switch(currentDigit)
	{
	case 0:
		DIGIT2_OFF();
		LATCH_HIGH();
		DIGIT0_ON();
		currentDigit = 1;
		break;
	case 1:
		DIGIT0_OFF();
		LATCH_HIGH();
		DIGIT1_ON();
		currentDigit = 2;
		break;
	case 2:
		DIGIT1_OFF();
		LATCH_HIGH();
		DIGIT2_ON();
		currentDigit = 0;
		break;
	default: currentDigit = 0; break;
	}
	LATCH_LOW();
	SPDR = displayBytes[currentDigit];
}

void displayInit(void)
{
	DISPLAY_RESET_HIGH();
	currentDigit = 0;
	SPDR = 0xFF;
}

void setNumber(int16_t integer, int8_t fractionalPart)
{
	// assume 3 digits display
	if(integer > 99 || integer < -99)
	{
		displayBytes[0] = segment1DigitValues[DISPLAY_UNDERSCORE];
		displayBytes[1] = segment2DigitValues[DISPLAY_UNDERSCORE];
		displayBytes[2] = segment1DigitValues[DISPLAY_UNDERSCORE];
		return;
	}
	uint8_t negative = 0;
	if(integer < 0 || fractionalPart < 0)
	{
		negative = 1;
		integer *= -1;
		fractionalPart *= -1;
	}
	int8_t digit1 = integer % 10;
	displayBytes[2] = segment1DigitValues[fractionalPart > 9 ? DISPLAY_UNDERSCORE : fractionalPart];
	displayBytes[1] = segment2DigitValues[digit1] & segment2DigitValues[DISPLAY_DOT];
	integer /= 10;
	int8_t digit0 = integer != 0 ? integer % 10 : DISPLAY_NOTHING;
	displayBytes[0] = segment1DigitValues[digit0];
	if(negative)
	{
		if(integer > 0)
		{
			displayBytes[2] = segment1DigitValues[digit1];
			displayBytes[1] = segment2DigitValues[digit0];
		}
		displayBytes[0] = segment1DigitValues[DISPLAY_MINUS];
	}
}

static inline void displayTemperature(int16_t temperature)
{
	setNumber(temperature / 10, temperature % 10);
}

void processDisplay(void)
{
	if(temperature < -2000)
	{
		if(displayState == DISPLAY_CURRENT_TEMP) displayState = DISPLAY_MISSING_SENSOR;
	}
	else
	{
		if(displayState == DISPLAY_MISSING_SENSOR) displayState = DISPLAY_CURRENT_TEMP;
	}

	switch(displayState)
	{
	case DISPLAY_CURRENT_TEMP:
		{
			BRIGHTNESS(255);
			displayTemperature(temperature);
		}
		break;
	case DISPLAY_MISSING_SENSOR:
		if(timCounter < 15)
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_MINUS];
			displayBytes[1] = segment2DigitValues[DISPLAY_MINUS];
			displayBytes[2] = segment1DigitValues[DISPLAY_MINUS];
		}
		else
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_NOTHING];
			displayBytes[1] = segment2DigitValues[DISPLAY_NOTHING];
			displayBytes[2] = segment1DigitValues[DISPLAY_NOTHING];
		}
		break;
	case DISPLAY_SET_TEMPERATURE:
		{
			if(!displayStateCounter)
			{
				displayState = DISPLAY_CURRENT_TEMP;
				break;
			}
			displayTemperature(temporarySetTemperature);

			if(timCounter < (TIM_SECOND_COUNTER_MAX / 2))
			{
				BRIGHTNESS(255 - (timCounter * (TIM_SECOND_COUNTER_MAX / 2 - 1)));
			}
			else
			{
				BRIGHTNESS(255 - ((TIM_SECOND_COUNTER_MAX - timCounter) * (TIM_SECOND_COUNTER_MAX / 2 - 1)));
			}
		}
		break;
	case DISPLAY_SET_MODE:
		{
			if(!displayStateCounter)
			{
				displayState = DISPLAY_CURRENT_TEMP;
				break;
			}
		}
		uint8_t dispCounter = temporaryMode == MODE_HEATING ? timCounter : TIM_SECOND_COUNTER_MAX - timCounter;
		if(dispCounter < (TIM_SECOND_COUNTER_MAX / 4))
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_DOT];
			displayBytes[1] = segment2DigitValues[DISPLAY_DOT];
			displayBytes[2] = segment1DigitValues[DISPLAY_DOT];
		}
		else if(dispCounter >= (TIM_SECOND_COUNTER_MAX / 4) && dispCounter < (TIM_SECOND_COUNTER_MAX / 2))
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_UNDERSCORE];
			displayBytes[1] = segment2DigitValues[DISPLAY_UNDERSCORE];
			displayBytes[2] = segment1DigitValues[DISPLAY_UNDERSCORE];
		}
		else if(dispCounter >= (TIM_SECOND_COUNTER_MAX / 4) && dispCounter < (TIM_SECOND_COUNTER_MAX * 3 / 4))
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_MINUS];
			displayBytes[1] = segment2DigitValues[DISPLAY_MINUS];
			displayBytes[2] = segment1DigitValues[DISPLAY_MINUS];
		}
		else if(dispCounter >= (TIM_SECOND_COUNTER_MAX * 3 / 4))
		{
			displayBytes[0] = segment1DigitValues[DISPLAY_UPPERLINE];
			displayBytes[1] = segment2DigitValues[DISPLAY_UPPERLINE];
			displayBytes[2] = segment1DigitValues[DISPLAY_UPPERLINE];
		}
		break;
	default:
		displayState = DISPLAY_CURRENT_TEMP;
		break;
	}

	if(OUTPUT_ENABLED)
	{
		if(timCounter < 3 || (timCounter > 6 && timCounter < 9))
		{
			displayBytes[2] &= segment1DigitValues[DISPLAY_DOT];
		}
	}
}

void displaySetState(uint8_t state)
{
	if(state == DISPLAY_SET_TEMPERATURE && displayState != state)
	{
		temporarySetTemperature = eeprom_read_word(&setTemperatureEEMEM);
		temporaryMode = eeprom_read_byte(&controlModeEEMEM);
	}

	displayState = state;
	displayStateCounter = 10;
}

void displaySecondElapsed(void)
{
	if(displayStateCounter) displayStateCounter--;
}
