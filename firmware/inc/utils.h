/*
 * utils.h
 *
 *  Created on: 19 wrz 2023
 *      Author: grzesiek
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <inttypes.h>

uint8_t crcUpdate(uint8_t crc, uint8_t data);
uint8_t calculateCrc8(uint8_t *input, uint8_t size);


#endif /* INC_UTILS_H_ */
