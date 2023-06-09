/*
 * AcelerometroDriver.c
 *
 *  Created on: May 25, 2023
 *      Author: ingfisica
 */

#include "stm32f4xx.h"
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include "AcelerometroDriver.h"


uint8_t i2c_dataBuffer = 0;
char usart_dataBuffer[128] = "Acelerometro AXL345 testing...";

void inicializacion_AXL345(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){	/* Mensaje de inicio */
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	/* Lectura e impresión de registro ID */
	device_ID(Usart_handler, accel_handler);

	/* Configuración a modo de medida*/
	measure_Mode_config(accel_handler);
	//constant_100Khz_measuring(accel_handler, timer100Khz_handler, pinTimer100khz);
	/* Confirmación de configuración */
	read_PowerMode(Usart_handler, accel_handler);
}

void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t x_position){

	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = (float) AccelX*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "X axis data = %.2f\n", ValorX);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	single_Data_archive->X_Data[x_position] = ValorX;
}

void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t y_position){

	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = AccelY*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Y axis data = %.2f\n", ValorY);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	single_Data_archive->Y_Data[y_position] = ValorY;
}

void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t z_position){

	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = (float) AccelZ*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Z axis data = %.2f\n", ValorZ);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	single_Data_archive->Z_Data[z_position] = ValorZ;
}

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Lectura e impresión de registro ID */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, ACCEL_ID);
	sprintf(usart_dataBuffer, "Who am I? (r) = %d\n", i2c_dataBuffer);
	interruptWriteMsg(Usart_handler, usart_dataBuffer);
}

void read_PowerMode(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Configurar a modo medida */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, POWER_CTL);

	/* Configuraciones encontradas posibles en el acelerador */
	if(i2c_dataBuffer == MEASURE_MODE){
		strcpy(usart_dataBuffer, "AXL345 measuring\n");
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	} else {
		strcpy(usart_dataBuffer, "Not configured\n");
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}
}

void measure_Mode_config(I2C_Handler_t *accel_handler){
 	/* Configuración a modo de medida*/
	i2c_writeSingleRegister(accel_handler, 0x2C,0xF);
	i2c_writeSingleRegister(accel_handler, POWER_CTL, MEASURE_MODE);

}

void constant_100Khz_measuring(I2C_Handler_t *accel_handler, BasicTimer_Handler_t *timer100Khz_handler,
								GPIO_Handler_t *pinTimer100khz){
	/* Configuración TIM4 para ser el control del muestreo a 1Khz*/
	timer100Khz_handler->ptrTIMx = TIM4;
	timer100Khz_handler->TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timer100Khz_handler->TIMx_Config.TIMx_speed = 1600;
	timer100Khz_handler->TIMx_Config.TIMx_period = 100; // 1 ms
	timer100Khz_handler->TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_DISABLE;
	BasicTimer_Config(timer100Khz_handler);

	pinTimer100khz->pGPIOx = GPIOA;
	pinTimer100khz->GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	pinTimer100khz->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	pinTimer100khz->GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinTimer100khz->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinTimer100khz->GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(pinTimer100khz);
}

void XYZ_dataset(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *xyz_Data_Archive, uint16_t set_position){
	/* Guarda las variables en la posición "set_position" y no las envía a través de
	 * USART individualmente */
	single_data_X(NULL, accel_handler, xyz_Data_Archive, set_position);
	single_data_Y(NULL,	accel_handler, xyz_Data_Archive, set_position);
	single_data_Z(NULL, accel_handler, xyz_Data_Archive, set_position);
	set_position++;
}

void print_All_Data(USART_Handler_t *Usart_handler, axis_Data_t *xyz_Data_Archive, uint16_t print_position){
	print_position = 0;
	sprintf(usart_dataBuffer, "3 Axis data\n(x ; y ; z)\n");
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	for(int i = print_position; i < BIGDATA_BUFFER; i++){
		sprintf(usart_dataBuffer, "(%.2f ; %.2f ; %.2f)\n",
					xyz_Data_Archive->X_Data[print_position],
					xyz_Data_Archive->Y_Data[print_position],
					xyz_Data_Archive->Z_Data[print_position]);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}
}

void print_XYZ_Data(axis_Data_t *xyz_Data, USART_Handler_t *Usart_handler, uint16_t print_position){
	sprintf(usart_dataBuffer, "(%.2f ; %.2f ; %.2f)\n",
			xyz_Data->X_Data[print_position],
			xyz_Data->Y_Data[print_position],
			xyz_Data->Z_Data[print_position]);
	interruptWriteMsg(Usart_handler, usart_dataBuffer);
}

float raw_data_X(I2C_Handler_t *accel_handler){
	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	uint16_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = (float) AccelX*(0.0039*9.8);

	return ValorX;
}

float raw_data_Y(I2C_Handler_t *accel_handler){
	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	uint16_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = (float) AccelY*(0.0039*9.8);

	return ValorY;
}

float raw_data_Z(I2C_Handler_t *accel_handler){
	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	uint16_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = (float) AccelZ*(0.0039*9.8);

	return ValorZ;
}


void read_XYZ_data(I2C_Handler_t *accel_handler, axis_Data_t *axis_Data, uint8_t acel_Pos){
	// Variables auxiliares
	uint8_t XAxis_Low = 0;
	uint8_t XAxis_High = 0;
	uint8_t YAxis_Low = 0;
	uint8_t YAxis_High = 0;
	uint8_t ZAxis_Low = 0;
	uint8_t ZAxis_High = 0;

	uint16_t Axis_full = 0;

	/* Generar condicion de inicio */
	i2c_startTransaction(accel_handler);

	/* Enviar dirección de dispositivo e indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(accel_handler, accel_handler->slaveAddress, I2C_WRITE_DATA);

	/* Enviar dirección de memoria a leer */
	i2c_sendMemoryAddress(accel_handler, ACCEL_XOUT_L);


	/* Crear condición de RESTART */
	i2c_reStartTransaction(accel_handler);

	/* Se vuelve a enviar la dirección con indicación de LECTURA */
	i2c_sendSlaveAddressRW(accel_handler, accel_handler->slaveAddress, I2C_READ_DATA);

	i2c_sendAck(accel_handler);

	XAxis_Low = i2c_readDataByte(accel_handler);
	XAxis_High = i2c_readDataByte(accel_handler);
	Axis_full = (XAxis_High << 8 | XAxis_Low) * (0.0039*9.8);
	axis_Data->X_Data[0] = Axis_full;

	YAxis_Low = i2c_readDataByte(accel_handler);
	YAxis_High = i2c_readDataByte(accel_handler);
	Axis_full = (YAxis_High << 8 | YAxis_Low) * (0.0039*9.8);
	axis_Data->X_Data[0] = Axis_full;

	ZAxis_Low = i2c_readDataByte(accel_handler);
	/* Generar condición NACK para que el esclavo solo envíe el último byte */
	i2c_sendNoAck(accel_handler);
	ZAxis_High = i2c_readDataByte(accel_handler);
	Axis_full = (ZAxis_High << 8 | ZAxis_Low) * (0.0039*9.8);

	axis_Data->X_Data[0] = Axis_full;


	/* Generar condición de parada */
	i2c_stopTransaction(accel_handler);
}



























