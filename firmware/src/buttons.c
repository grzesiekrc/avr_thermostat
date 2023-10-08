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

#define DEBOUNCE_LIMIT 10

void buttonUp(void);
void buttonDown(void);
void buttonEnter(void);

button_t enterButton = {.counter = 0, .pressAction = buttonEnter};
button_t upButton = {.counter = 0, .pressAction = buttonUp};
button_t downButton = {.counter = 0, .pressAction = buttonDown};

volatile uint8_t received = 0;

ISR(USART_RXC_vect)
{
	received = UDR;
}

ISR(USART_TXC_vect)
{

}

void buttonUp(void)
{
	displaySetState(DISPLAY_SET_TEMPERATURE);
	if(temporarySetTemperature < 1000) temporarySetTemperature++;
}

void buttonDown(void)
{
	displaySetState(DISPLAY_SET_TEMPERATURE);
	if(temporarySetTemperature > -300) temporarySetTemperature--;
}

void buttonEnter(void)
{
	if(displayState == DISPLAY_SET_TEMPERATURE)
	{
		displayState = DISPLAY_CURRENT_TEMP;
		configureTemperature(temporarySetTemperature);
	}
}

static inline void detectPress(uint8_t pinState, button_t* button)
{
	if(pinState) // released
	{
		button->counter = 0;
		button->actionExecuted = 0;
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
