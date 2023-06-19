/*
 * Funciones.h
 *
 *  Created on: Jun 17, 2023
 *      Author: ingfisica
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
//#include "ov7670.h"
#include "RegOv7670.h"




#define CAPTURANDO 	0
#define STANDBY 	1

void Led_estado(GPIO_TypeDef *pGPIO, uint16_t pPIN);
uint8_t ov7670_write(I2C_HandleTypeDef *phi2c,uint8_t regADDR, uint8_t sentData);
uint8_t ov7670_read(I2C_HandleTypeDef *phi2c, uint8_t regADDR, uint8_t *readData);
uint8_t ov7670_init(DCMI_HandleTypeDef *phdcmi, DMA_HandleTypeDef *phdma,
					UART_HandleTypeDef *phuart, I2C_HandleTypeDef *phi2c);
uint8_t ov7670_StartCap(uint32_t direccionDestino);
void ov7670_reset(I2C_HandleTypeDef *phi2c);
#endif /* FUNCIONES_H_ */
