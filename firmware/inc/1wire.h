/*
 * 1wire.h
 *
 *  Created on: 19 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_1WIRE_H_
#define INC_1WIRE_H_

#include <inttypes.h>

#define DQPIN	PD4

#define IN_DQ (PIND & (1<<DQPIN))

#define PINS_INPUT()	DDRD &= ~(1<<DQPIN)
#define PINS_OUTPUT()	DDRD |= (1<<DQPIN)
#define PINS_HIGH()	PORTD |= (1<<DQPIN)
#define PINS_LOW()	PORTD &= ~(1<<DQPIN)

int16_t readTemperature(void);

enum tempConstants{tempSensorInactiveValue = -2090, tempInvalidValue = -3000, tempCommunicationError = -3010, tempUnknownSensor = -3020};
enum tempSensorVersion{sensorUnknown = 0, sensorDS18B20 = 0x28, sensorDS18S20 = 0x10};


#endif /* INC_1WIRE_H_ */
