/*
 * Funciones.c
 *
 *  Created on: Jun 17, 2023
 *      Author: SMontoya
 *
 */
#include "Funciones.h"
#include "main.h"

HAL_StatusTypeDef HAL_check; // Chequeo de funcionamiento función HAL
static char uart_buffer[64];
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

	HAL_check = HAL_I2C_Mem_Write(phi2c, OV7670_WRITE, regADDR, 1, &sentData, 1, 100);
//	while(HAL_check != HAL_OK);
	return HAL_check;
}

uint8_t ov7670_read(I2C_HandleTypeDef *phi2c, uint8_t regADDR, uint8_t *readData){

	HAL_check = HAL_I2C_Master_Transmit(phi2c, OV7670_WRITE , &regADDR, 1, 100);
//	while(HAL_check != HAL_OK);
	/* Mem read no funciona por el protocolo SCCB, al no tener ACK se atrapa */
	HAL_check |= HAL_I2C_Master_Receive(phi2c, OV7670_READ, readData, 1, 100);
//	while(HAL_check != HAL_OK);

	return HAL_check;
}

void ov7670_reset(I2C_HandleTypeDef *phi2c){

	ov7670_write(phi2c, REG_COM7, COM7_RESET);
	HAL_Delay(30);
	/* Datasheet indica el tiempo que tarda en procesar el reset */
}

uint8_t ov7670_init(DCMI_HandleTypeDef *phdcmi, DMA_HandleTypeDef *phdma,
					UART_HandleTypeDef *phuart, I2C_HandleTypeDef *phi2c){

	/* Asegurar encendido de pin conectado a RESET, los delays aseguran
	 * la estabilidad del sistema después de apagar y encender para seguir
	 * con la configuración */
	HAL_GPIO_WritePin(Cam_RESET_GPIO_Port, Cam_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(Cam_RESET_GPIO_Port, Cam_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	ov7670_reset(phi2c);

	/* Confirmar interacción */
	uint8_t buffer[4];
	ov7670_read(phi2c, REG_VER, buffer);
	sprintf(uart_buffer,"[OV7670] VER (0x73) = 0x%X\n", buffer[0]);
	HAL_UART_Transmit(phuart, (uint8_t *)uart_buffer, strlen(uart_buffer), 10);

	ov7670_read(phi2c, REG_PID, buffer);
	sprintf(uart_buffer,"[OV7670] PID (0x76) = 0x%X\n", buffer[0]);
	HAL_UART_Transmit(phuart, (uint8_t *)uart_buffer, strlen(uart_buffer), 10);

	sprintf(uart_buffer,"Init OV7670 melo\n");
	HAL_UART_Transmit(phuart, (uint8_t *)uart_buffer, strlen(uart_buffer), 10);

	return HAL_OK;
}

//uint8_t ov7670_config(void){
//
//}

//uint8_t ov7670_StartCap(uint32_t direccionDestino){
////	HAL_check = HAL_DCMI_Start_DMA(pFlash, DCMI_MODE_SNAPSHOT, direccionDestino, Length);
////	return HAL_check;
//}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *phdcmi){
//	HAL_check = HAL_DMA_Start_IT(phdcmi->DMA_Handle, (uint32_t) &phdcmi->Instance->DR,
//			/* memoria de destino */, /*Longitud de los datos*/);
}
