/*
 * main.c
 *
 *  Created on: 14 wrz 2023
 *      Author: grzesiek
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "1wire.h"
#include "display.h"
#include "output_control.h"

volatile uint8_t received = 0;
int16_t temperature = 0;

volatile uint8_t timCounter;
volatile uint8_t seconds = 0;

ISR(USART_RXC_vect)
{
	received = UDR;
}

ISR(USART_TXC_vect)
{

}

ISR(TIMER0_OVF_vect)
{
	if(++timCounter > 30)
	{
		seconds++;
		timCounter = 0;
	}
}

void sendByte(uint8_t byte)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = byte;
}

void buttonUp(void)
{

}

void buttonDown(void)
{

}

void buttonEnter(void)
{

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
}

void printTemperature(int16_t value)
{
	if (value < 0)
	{
		sendByte('-');
		value *= -1;
	}

	uint8_t digits[10];
	int8_t i = 0;

	while (value > 0)
	{
		digits[i++] = (value % 10) + '0';
		value /= 10;
	}

	i--;

	while (i > 0)
	{
		sendByte(digits[i--]);
	}
	sendByte('.');
	sendByte(digits[0]);
	sendByte('*');//Celsius degree sign
	sendByte('C');
}

void processTimer(void)
{
	static uint16_t prevSeconds;
	if(prevSeconds != seconds)
	{
		prevSeconds = seconds;
		sendByte('\r');
		printTemperature(temperature);
		sendByte(' ');
		sendByte(' ');
		sendByte(' ');
	}
}

int main(void)
{
	DDRB = (1<<PB0) | (1<<PB2) | (1<<PB3) | (1<<PB5);
	DDRC = (1<<PC0) | (1<<PC1);
	DDRD = (1<<PD1) | (1<<PD2) | (1<<PD3) | (1<<PD6);
	PORTD = (1<<PD5) | (1<<PD7);

	UBRRH = 0;
	UBRRL = (unsigned char)51;

	//UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<TXCIE) | (1<<RXCIE) | (1<<UCSZ2);
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);

	TCCR0 = (1<<CS02) | (1<<CS00);

	TCCR1A = 0;
	TCCR1B = (1<<CS11); // 8 prescaler = 1 MHz - increment 1us

	TIMSK = (1<<TOIE0);

	SPCR = (1<<SPIE) | (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0);

	sei();

	displayInit();

	while(1)
	{
		temperature = readTemperature();
		temperatureNotification(temperature);
		processButtons();
		processTimer();
		processDisplay();
	}
}
