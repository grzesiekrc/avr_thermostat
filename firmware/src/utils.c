/*
 * utils.c
 *
 *  Created on: 19 wrz 2023
 *      Author: grzesiek
 */

#include "utils.h"

uint8_t crcUpdate(uint8_t crc, uint8_t data)
{
	uint8_t i;

	crc ^= data;
	for (i = 0; i < 8; i++)
	{
		if (crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
		else
			crc >>= 1;
	}

	return crc;
}

uint8_t calculateCrc8(uint8_t *input, uint8_t size)
{
	uint8_t i, crc=0;

	size--;

	for( i = 0 ; i < size ; i++ )
	{
		crc = crcUpdate(crc, input[i]);
	}

	return crc;
}
