/**************************************************************************//**
This SPI driver is used to interface the TI ADS8681 ADC chip.

   \date          26-April-2024
   \copyright     2024 Thorlabs Spectral Works
   \author       Nicholas Junker (njunker@thorlabs.com)

******************************************************************************/

#ifndef INC_ADS8681_LIB_H_
#define INC_ADS8681_LIB_H_

#include <cstdint> //for uint8_t, etc
#include <cstddef> //for size_t

#define ADC_SPI_NOP  0b0000000
#define ADC_SPI_READ_HWORD  0b1100100
#define ADC_SPI_READ  0b0100100
#define ADC_SPI_WRITE_FULL  0b1101000
#define ADC_SPI_SET_HWORD  0b1101100

#define ADC_RANGE_SEL_REGISTER 0x14

class Thorlabs_ADS8681 {
public:
	//TODO make some nicer functions like input range set, checking alarm status, etc

	typedef enum {
		POS_NEG_12V28 = 0x00,
		POS_NEG_10V24 = 0x01,
		POS_NEG_6V144 = 0x02,
		POS_NEG_5V12 = 0x03,
		POS_NEG_2V56 = 0x04,
		POS_12V28 = 0x08,
		POS_10V24 = 0x09,
		POS_6V144 = 0x0A,
		POS_5V12 = 0x0B
	} ADC_inputRange;

	//Initialize ADC parameters.
	void begin();

	//Set the ADC input range.
	void set_inputRange(ADC_inputRange range);

	//Acquire full ADC data & status bits.
	uint16_t acquire(int32_t* out);
	
	//Acquire just ADC data as fast as possible.
	uint16_t acquire_fast();

	//Acquire ADC data, then calculate voltage.
	float calculateVoltage();

	float FSR;
	float lsb;

protected:

	//write to a single register
	void write_register(uint8_t addr, uint16_t data);

	//read from a single register
	uint16_t read_register(uint8_t addr, int32_t* out);
	
	int8_t _cs, _mosi, _miso, _sck, _nrst;

	//Our own SPI transfer to facilitate different platforms
	virtual void Thorlabs_SPI_transfer(void *buf, size_t count);

	//User-implemented SPI begin function, if needed
	virtual void Thorlabs_SPI_begin();

	//User-implemented SPI end function, if needed
	virtual void Thorlabs_SPI_end();

	//User-implemented SPI setup function, if needed
	virtual void Thorlabs_SPI_setup();
	
};

#endif /* INC_ADS8681_LIB_H_ */
