/*
 * output_control.h
 *
 *  Created on: 21 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_OUTPUT_CONTROL_H_
#define INC_OUTPUT_CONTROL_H_

#include <avr/eeprom.h>

#define OUTPUT_OFF()	PORTD |= (1<<PD3);
#define OUTPUT_ON()	PORTD &= ~(1<<PD3);
#define OUTPUT_ENABLED (!(PIND & (1<<PD3)))

extern uint16_t EEMEM setTemperatureEEMEM;
extern uint8_t EEMEM controlModeEEMEM;

enum thermostat_mode_t{MODE_HEATING, MODE_COOLING};

void temperatureNotification(int16_t temperature);
void configureTemperature(int16_t temperature);
void configureMode(uint8_t mode);

#endif /* INC_OUTPUT_CONTROL_H_ */
