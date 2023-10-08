/*
 * 1wire.c
 *
 *  Created on: 19 wrz 2023
 *      Author: grzesiek
 */

#include <util/delay.h>
#include <avr/io.h>

#include "1wire.h"
#include "utils.h"

static uint8_t reset1wire(void)
{
	uint8_t sensor_presence = 0;
	PINS_LOW();
	PINS_OUTPUT();
	_delay_us(255);
	_delay_us(255);
	PINS_INPUT();
	_delay_us(160);
	if(IN_DQ) sensor_presence |= (1 << 0);
	_delay_us(255);
	_delay_us(100);
	return sensor_presence;
}

static uint8_t readByte1wire(void)
{
	uint8_t i;
	uint8_t byte = 0;

	for (i = 8; i; i--)
	{
		byte >>= 1;
		PINS_OUTPUT();
		PINS_LOW();
		_delay_us(2);
		PINS_INPUT();
		_delay_us(4);
		if(IN_DQ) byte |= 0b10000000;
		_delay_us(30);
	}
	return byte;
}

static void sendByte1wire(uint8_t val)
{
	uint8_t i;
	for (i = 8; i; i--)
	{
		PINS_OUTPUT();
		PINS_LOW();
		_delay_us(2);
		if(val & 1) PINS_HIGH();
		_delay_us(30);
		PINS_INPUT();
		val >>= 1;
	}
	PINS_HIGH();
	PINS_OUTPUT();
}

static int16_t processSensor(uint8_t *data, uint8_t type)
{
	uint8_t crc = 0, i;

	for(i = 0; i < 8; i++)
	{
		crc = crcUpdate(crc, data[i]);
	}

	if(crc != data[8])
	{
		return tempInvalidValue;
	}

	if(type == sensorDS18B20)
	{
		int16_t fractional_part = (data[0] & 0x0F) * 625;//0,0625 - 1bit resolution
		data[0] >>= 4;
		data[1] <<= 4;
		int8_t decimal_part = data[1] | data[0];
		if(decimal_part < 0)
		{
			fractional_part = 10000 - fractional_part;
		}
		int8_t fraction = fractional_part / 1000;
		if(fractional_part % 1000 > 500)//round up
		{
			fraction++;
		}

		if(decimal_part < 0)
		{
			decimal_part++;
			decimal_part *= -1;
			return (decimal_part * 10 + fraction) * -1;
		}
		else
		{
			return decimal_part * 10 + fraction;
		}
	}
	else if(type == sensorDS18S20)
	{
		int8_t fraction = data[0] & 1;
		data[0] >>= 1;
		int8_t decimal_part = data[0];
		if (data[1]) decimal_part |= 0x80;
		int16_t result = decimal_part * 10;
		if (fraction) result += 5;
		return result;
	}
	else
	{
		return tempUnknownSensor;
	}
}

int16_t readTemperature(void)
{
	uint8_t i;
	uint8_t sensorType;
	uint8_t sensor[9];

	reset1wire();
	sendByte1wire(0xCC);
	sendByte1wire(0x44);

	reset1wire();
	sendByte1wire(0x33);

//	for(i = 0; i < 8; i++)
//	{
//		sensor[i] = readByte1wire();
//	}
//
//	sensorType = sensor[0];
	sensorType = readByte1wire();

	reset1wire();
	sendByte1wire(0xCC);
	sendByte1wire(0xBE);

	for(i = 0; i < 9; i++)
	{
		sensor[i] = readByte1wire();
	}

	return processSensor(sensor, sensorType);
}
