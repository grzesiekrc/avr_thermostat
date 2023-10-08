/*
 * display.c
 *
 *  Created on: 20 wrz 2023
 *      Author: grzesiek
 */

#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define OUTPUT_ENABLED (!(PIND & (1<<PD3)))

extern int16_t temperature;
extern volatile uint8_t timCounter;

enum displayState_t
{
	DISPLAY_CURRENT_TEMP,
	DISPLAY_MISSING_SENSOR
};

uint8_t displayState;

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
	0xFB // _
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
	0xF7 // _
};

#define DISPLAY_MINUS	10
#define DISPLAY_DOT		11
#define DISPLAY_NOTHING	12
#define DISPLAY_UNDERSCORE 13

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

void setNumber(int16_t integer, uint8_t decimalPart)
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
	if(integer < 0)
	{
		negative = 1;
		integer *= -1;
	}
	int8_t digit1 = integer % 10;
	displayBytes[2] = segment1DigitValues[decimalPart > 9 ? DISPLAY_UNDERSCORE : decimalPart];
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

void processDisplay(void)
{
	if(temperature < -2000)
	{
		displayState = DISPLAY_MISSING_SENSOR;
	}
	else
	{
		displayState = DISPLAY_CURRENT_TEMP;
	}

	switch(displayState)
	{
	case DISPLAY_CURRENT_TEMP:
		{
			int8_t fractionalPart = temperature % 10;
			if(fractionalPart < 0) fractionalPart *= -1;
			setNumber(temperature / 10, fractionalPart);
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
	}

	if(OUTPUT_ENABLED)
	{
		if(timCounter < 3 || (timCounter > 6 && timCounter < 9))
		{
			displayBytes[2] &= segment1DigitValues[DISPLAY_DOT];
		}
	}
}
