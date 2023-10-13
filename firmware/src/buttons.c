/*
 * buttons.c
 *
 *  Created on: 8 paź 2023
 *      Author: grzesiek
 */

#include <avr/interrupt.h>
#include "buttons.h"
#include "display.h"
#include "output_control.h"

#define DEBOUNCE_LIMIT 4
#define HOLD_INTERVAL	2
#define HOLD_WAIT	100

void buttonUp(void);
void buttonDown(void);
void buttonEnter(void);
void holdUp(void);
void holdDown(void);
void holdEnter(void);

button_t enterButton = {.counter = 0, .pressAction = buttonEnter, .holdEvent = holdEnter};
button_t upButton = {.counter = 0, .pressAction = buttonUp, .holdEvent = holdUp};
button_t downButton = {.counter = 0, .pressAction = buttonDown, .holdEvent = holdDown};

volatile uint8_t received = 0;

ISR(USART_RXC_vect)
{
	received = UDR;
}

ISR(USART_TXC_vect)
{

}

static inline void changeTempMode(void)
{
	temporaryMode = temporaryMode == MODE_HEATING ? MODE_COOLING : MODE_HEATING;
	displaySetState(DISPLAY_SET_MODE);
}

void buttonUp(void)
{
	if(displayState == DISPLAY_SET_TEMPERATURE || displayState == DISPLAY_CURRENT_TEMP)
	{
		displaySetState(DISPLAY_SET_TEMPERATURE);
		if(temporarySetTemperature < 1000) temporarySetTemperature++;
	}
	else if(displayState == DISPLAY_SET_MODE)
	{
		changeTempMode();
	}
}

void buttonDown(void)
{
	if(displayState == DISPLAY_SET_TEMPERATURE || displayState == DISPLAY_CURRENT_TEMP)
	{
		displaySetState(DISPLAY_SET_TEMPERATURE);
		if(temporarySetTemperature > -300) temporarySetTemperature--;
	}
	else if(displayState == DISPLAY_SET_MODE)
	{
		changeTempMode();
	}
}

void buttonEnter(void)
{
	if(displayState == DISPLAY_SET_TEMPERATURE)
	{
		displayState = DISPLAY_CURRENT_TEMP;
		configureTemperature(temporarySetTemperature);
	}
	else if(displayState == DISPLAY_SET_MODE)
	{
		displayState = DISPLAY_CURRENT_TEMP;
		configureMode(temporaryMode);
	}
}

void holdUp(void)
{
	if(DISPLAY_SET_TEMPERATURE == displayState)
	{

		buttonUp();
	}
}

void holdDown(void)
{
	if(DISPLAY_SET_TEMPERATURE == displayState)
	{
		buttonDown();
	}
}

void holdEnter(void)
{
	if(DISPLAY_CURRENT_TEMP == displayState)
	{
		displaySetState(DISPLAY_SET_MODE);
	}
}

static inline void detectPress(uint8_t pinState, button_t* button)
{
	if(pinState) // released
	{
		button->counter = 0;
		button->actionExecuted = 0;
		button->holdCounter = -HOLD_WAIT;
	}
	else // pressed
	{
		if(button->counter < DEBOUNCE_LIMIT)
		{
			button->counter++;
		}
		else
		{
			if(!button->actionExecuted)
			{
				button->actionExecuted = 1;
				button->pressAction();
			}
			if(++button->holdCounter > HOLD_INTERVAL)
			{
				button->holdCounter = 0;
				button->holdEvent();
			}
		}
	}
}

void processButtons(void)
{
	switch(received)
	{
	case 'w': buttonUp(); break;
	case 's': buttonDown(); break;
	case 'z': buttonEnter(); break;
	}
	received = 0;

	detectPress(PIND & (1<<PD7), &upButton);
	detectPress(PIND & (1<<PD5), &downButton);
	detectPress(PINB & (1<<PB1), &enterButton);
}
