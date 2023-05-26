/*
 * AcelerometroDriver.c
 *
 *  Created on: May 25, 2023
 *      Author: ingfisica
 */

#include "stm32f4xx.h"
#include <string.h>
#include <float.h>
#include "AcelerometroDriver.h"
#include "USARTxDriver.h"

uint8_t i2c_dataBuffer = 0;
uint8_t usart_dataBuffer[128] = "Accel MPU-6050 testing...";

void inicializacion_AXL345(USART_Handler_t *Usartcomunication, I2C_Handler_t *AXL345){
	/* Mensaje de inicio */
	interruptWriteMsg(&Usartcomunication, usart_dataBuffer);

	/* Lectura e impresión de registro ID */
	i2c_dataBuffer = i2c_readSingleRegister(AXL345, ACCEL_ID);
	strcpy(usart_dataBuffer, "Who am I? (r) = %d\n", (unsigned int)i2c_dataBuffer);
	interruptWriteMsg(&Usartcomunication, usart_dataBuffer);

	/* Configuración a modo de medida*/
	strcpy(usart_dataBuffer, "Power control\n");
	interruptWriteMsg(Usartcomunication, usart_dataBuffer);
	i2c_writeSingleRegister(AXL345, POWER_CTL, MEASURE_MODE);
	i2c_dataBuffer = i2c_readSingleRegister(AXL345, i2c_dataBuffer,POWER_CTL);

	if(i2c_dataBuffer == MEASURE_MODE){
		strcpy(usart_dataBuffer, "AXL345 measuring");
		interruptWriteMsg(&Usartcomunication, usart_dataBuffer);
	} else {
		strcpy(usart_dataBuffer, "Not configured");
		interruptWriteMsg(&Usartcomunication, usart_dataBuffer);
	}
}


