/*
 * buttons.h
 *
 *  Created on: 8 paź 2023
 *      Author: grzesiek
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

typedef struct
{
	uint16_t counter;
	int16_t holdCounter;
	uint8_t actionExecuted;
	void (*pressAction)(void);
	void (*holdEvent)(void);
} button_t;

void processButtons(void);


#endif /* INC_BUTTONS_H_ */
