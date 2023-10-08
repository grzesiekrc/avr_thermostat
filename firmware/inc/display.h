/*
 * display.h
 *
 *  Created on: 20 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <inttypes.h>

#define DISPLAY_DIGITS	3
#define DIGIT2_OFF()	PORTB |= (1<<PB0)
#define DIGIT1_OFF()	PORTD |= (1<<PD6)
#define DIGIT0_OFF()	PORTD |= (1<<PD2)
#define DIGIT2_ON()	PORTB &= ~(1<<PB0)
#define DIGIT1_ON()	PORTD &= ~(1<<PD6)
#define DIGIT0_ON()	PORTD &= ~(1<<PD2)

#define LATCH_HIGH()	PORTC |= (1<<PC1)
#define LATCH_LOW()	PORTC &= ~(1<<PC1)


#define DISPLAY_RESET_LOW()	PORTC &= ~(1<<PC0)
#define DISPLAY_RESET_HIGH()	PORTC |= (1<<PC0)


void displayInit(void);
void setNumber(int16_t integer, uint8_t decimalPart);
void processDisplay(void);

#endif /* INC_DISPLAY_H_ */
