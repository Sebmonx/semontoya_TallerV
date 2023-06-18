/*
 * Funciones.c
 *
 *  Created on: Jun 17, 2023
 *      Author: SMontoya
 *
 */
#include "Funciones.h"


HAL_StatusTypeDef HAL_check; // Chequeo de funcionamiento función HAL
uint8_t pickLed = 0;

void Led_estado(GPIO_TypeDef *pGPIO, uint16_t pPIN){

	switch (pickLed) {
		case 0:
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			pickLed++;
			break;

		case 1:
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			pickLed++;
			break;

		case 2:
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			pickLed = 0;
			break;
		default:
			break;
	}

	HAL_GPIO_TogglePin(pGPIO, pPIN);
}


uint8_t ov7670_write(I2C_HandleTypeDef *phi2c,uint8_t regADDR, uint8_t sentData){

	HAL_check = HAL_I2C_Mem_Write(hi2c, OV7670, regADDR, MemAddSize, &sentData, 1, 100);

	return HAL_check;
}

uint8_t ov7670_read(I2C_HandleTypeDef *phi2c, uint8_t regADDR, uint8_t *readData){

	HAL_check = HAL_I2C_Master_Transmit(phi2c, OV7670, &regADDR, 1, 100);
	HAL_check |= HAL_I2C_Master_Receive(phi2c, OV7670, readData, 1, 100);

	return HAL_check;
}


uint8_t ov7670_init(DCMI_HandleTypeDef *phdcmi, DMA_HandleTypeDef *phdma, I2C_HandleTypeDef *phi2c){


}
