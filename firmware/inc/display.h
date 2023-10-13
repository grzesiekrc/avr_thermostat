/*
 * display.h
 *
 *  Created on: 20 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <inttypes.h>

#define BRIGHTNESS(x)	OCR1B = (x)

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

enum displayState_t
{
	DISPLAY_CURRENT_TEMP,
	DISPLAY_MISSING_SENSOR,
	DISPLAY_SET_TEMPERATURE,
	DISPLAY_SET_MODE
};

extern uint8_t displayState;
extern int16_t temporarySetTemperature;
extern int16_t temporaryMode;

void displaySetState(uint8_t state);
void displayInit(void);
void processDisplay(void);
void displaySecondElapsed(void);

#endif /* INC_DISPLAY_H_ */
