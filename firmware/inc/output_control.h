/*
 * output_control.h
 *
 *  Created on: 21 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_OUTPUT_CONTROL_H_
#define INC_OUTPUT_CONTROL_H_

#include <inttypes.h>

enum thermostat_mode_t{MODE_HEATING, MODE_COOLING};

void temperatureNotification(int16_t temperature);
void configureTemperature(int16_t temperature);
void configureMode(uint8_t mode);

#endif /* INC_OUTPUT_CONTROL_H_ */
