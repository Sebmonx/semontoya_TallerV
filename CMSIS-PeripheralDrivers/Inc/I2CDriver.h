/*
 * I2CDriver.h
 *
 *  Created on: 10/05/2023
 *      Author: Sebastian Montoya
 */

#ifndef I2CDRIVER_H_
#define I2CDRIVER_H_

#include <stm32f4xx.h>

#define I2C_WRITE_DATA 	0
#define I2C_READ_DATA 	1

#define MAIN_CLOCK_4_MHz_FOR_I2C	4
#define MAIN_CLOCK_16_MHz_FOR_I2C	16
#define MAIN_CLOCK_20_MHz_FOR_I2C	20
#define APB1_CLOCK_40MHz_FOR_I2C 	40

#define I2C_MODE_SM		0
#define I2C_MODE_FM		1

#define I2C_MODE_SM_16Mhz		80
#define I2C_MODE_FM_16Mhz		14
#define I2C_MODE_SM_80Mhz		200
#define I2C_MODE_FM_80Mhz		33

#define I2C_MAX_TRISE_SM_16Mhz		17
#define I2C_MAX_TRISE_FM_16Mhz		5
#define I2C_MAX_TRISE_SM_80Mhz		41
#define I2C_MAX_TRISE_FM_80Mhz		13




typedef struct
{
	uint8_t			MCU_frequency;
	I2C_TypeDef		*ptrI2Cx;
	uint8_t			slaveAddress;
	uint8_t			modeI2C;
	uint8_t			dataI2C;
} I2C_Handler_t;


void i2c_config(I2C_Handler_t *ptrHandlerI2C);
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite);
void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memoryAddress);
void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C);
uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t registerToRead);
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue);










#endif /* I2CDRIVER_H_ */
