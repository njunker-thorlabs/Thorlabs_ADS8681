/*
 * ADS8681_lib.c
 *
 *  Created on: Apr 26, 2024
 *      Author: NJunker
 *
 *      WIP library for the ADS8681
 */

#include "ADS8681_lib.h"


void Thorlabs_ADS8681::begin()
{	
	Thorlabs_SPI_setup();
}


void Thorlabs_ADS8681::set_inputRange(ADC_inputRange range)
{
	write_register(ADC_RANGE_SEL_REGISTER, range);

	switch (range) {
		case POS_NEG_12V28:
			FSR = 24.576f;
			lsb = 375e-6f;
			break;
		case POS_NEG_10V24:
			FSR = 20.48f;
			lsb = 312.5e-6f;
			break;
		case POS_NEG_6V144:
			FSR = 12.288f;
			lsb = 187.5e-6f;
			break;
		case POS_NEG_5V12:
			FSR = 10.24f;
			lsb = 156.25e-6f;
			break;
		case POS_NEG_2V56:
			FSR = 5.12f;
			lsb = 78.125e-6f;
			break;
		case POS_12V28:
			FSR = 12.288f;
			lsb = 187.5e-6f;
			break;
		case POS_10V24:
			FSR = 10.24f;
			lsb = 156.25e-6f;
			break;
		case POS_6V144:
			FSR = 6.144f;
			lsb = 93.75e-6f;
			break;
		case POS_5V12:
			FSR = 5.12f;
			lsb = 78.125e-6f;
			break;
		default:
			FSR = 5.12f;
			lsb = 78.125e-6f;
		}
}

void Thorlabs_ADS8681::write_register(uint8_t addr, uint16_t data)
{
	const int buf_size = 4;
	uint8_t cmd[buf_size];
	//build command word
	cmd[0] = (ADC_SPI_WRITE_FULL << 1);
	cmd[1] = (addr & 0xFF);
	cmd[2] = data >> 8;
	cmd[3] = data;
	
	//Begin Transaction
	Thorlabs_SPI_begin();

	Thorlabs_SPI_transfer(cmd, buf_size);
	
	Thorlabs_SPI_end();
}

uint16_t Thorlabs_ADS8681::read_register(uint8_t addr, int32_t* out)
{
	const int buf_size = 4;
	uint8_t cmd[buf_size];
	uint8_t dummy_cmd[buf_size];
	
	//build command word
	cmd[0] = (ADC_SPI_READ << 1);
	cmd[1] = (addr & 0xFF);
	cmd[2] = 0x00;
	cmd[3] = 0x00;

	for (int i = 0; i < buf_size; i++) {
		dummy_cmd[i] = cmd[i];
	}

	//Begin Transaction
	Thorlabs_SPI_begin();

	//Transfer dummy cmd to set our read register
	Thorlabs_SPI_transfer(dummy_cmd, buf_size);

	//Receive read register into dummy cmd
	Thorlabs_SPI_transfer(cmd, buf_size);

	//End transaction
	Thorlabs_SPI_end();


	//Go ahead and format int32 from our 4 bytes
	int32_t _out = ((int32_t) cmd[0]) << 24;
	_out |= ((int32_t) cmd[1]) << 16;
	_out |= ((int32_t) cmd[2]) << 8;
	_out |= ((int32_t) cmd[3]);
	*out = _out;

	return _out >> 16; //bits 31-16 are conversion result
}

uint16_t Thorlabs_ADS8681::acquire(int32_t* out)
{
	//Make command and dummy command all zero
	const int buf_size = 4;
	uint8_t cmd[buf_size] = {0x00};
	uint8_t dummy_cmd[buf_size] = {0x00};

	//Begin Transaction
	Thorlabs_SPI_begin();

	//Transfer data with dummy cmd
	Thorlabs_SPI_transfer(dummy_cmd, buf_size);

	//Transfer cmd, receive data into cmd buffer
	Thorlabs_SPI_transfer(cmd, buf_size);

	//Go ahead and format int32 from our 4 bytes
	int32_t _out = ((int32_t) cmd[0]) << 24;
	_out |= ((int32_t) cmd[1]) << 16;
	_out |= ((int32_t) cmd[2]) << 8;
	_out |= ((int32_t) cmd[3]);
	*out = _out;
	
	//End transaction
	Thorlabs_SPI_end();

	return _out >> 16; //bits 31-16 are conversion result
}

uint16_t Thorlabs_ADS8681::acquire_fast()
{
	Thorlabs_SPI_begin();
	
	const int buf_size = 2;
	uint8_t cmd[buf_size];
	
	cmd[0] = 0x00;
	cmd[1] = 0x00;
	
	//No building command word, just dummy data

	//This is NOT a properly formatted command like in ADC_acquire, this is as short
	//as possible for max acquisition speed. Because of this, each RX data will be off by
	//one from when requested and we won't get the full 32 byte frame.
	
	//Transmit dummy data, receive sample bytes
	Thorlabs_SPI_transfer(cmd, buf_size);
	
	//End transaction
	Thorlabs_SPI_end();

	//Return a nice formatted uint16, will convert to float in main loop
	return (((int16_t) cmd[0]) << 8 | (int16_t) cmd[1]);
}

float Thorlabs_ADS8681::calculateVoltage()
{
	float voltage = (float)acquire_fast();
	return voltage * lsb;
}

//-----------------------------------------------------------------------
//------------------- To be implemented by user -------------------------
//-----------------------(Platform Specific)-----------------------------
//-----------------------------------------------------------------------

void Thorlabs_ADS8681::Thorlabs_SPI_transfer(void *buf, size_t count) {
	//Implement this in a parent class or modify for your platform
	
	//Take in an array of single bytes (buf) of size (count)
	//Replace the transmitted bytes with the received data
}

void Thorlabs_ADS8681::Thorlabs_SPI_begin() {
	//Implement this in a parent class or modify for your platform

	//Used if your platform has an SPI transaction begin function (i.e. Arduino)
}

void Thorlabs_ADS8681::Thorlabs_SPI_end() {
	//Implement this in a parent class or modify for your platform

	//Used if your platform has an SPI transaction end function (i.e. Arduino)
}

void Thorlabs_ADS8681::Thorlabs_SPI_setup() {
	//Implement this in a parent class or modify for your platform

	//Platform specific startup code, i.e. pin assignments / SPI initialization
}