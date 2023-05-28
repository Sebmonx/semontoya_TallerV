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
char usart_dataBuffer[128] = "Accel AXL345 testing...";

void inicializacion_AXL345(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
							char *data_Buffer){
	/* Mensaje de inicio */
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	/* Lectura e impresión de registro ID */
	device_ID(Usart_handler, accel_handler,data_Buffer);

	/* Configuración a modo de medida*/
	measure_Mode_config(accel_handler);
	//constant_100Khz_measuring(accel_handler, timer100Khz_handler, pinTimer100khz);
	/* Confirmación de configuración */
	read_PowerMode(Usart_handler, accel_handler,data_Buffer);
}

void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t x_position, char *data_Buffer){

	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = (float) AccelX*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(data_Buffer, "X axis data = %.2f\n", ValorX);
		interruptWriteMsg(Usart_handler, data_Buffer);
	}

	if(single_Data_archive != NULL){
		single_Data_archive->X_Data[x_position] = ValorX;
	}
}

void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t y_position, char *data_Buffer){

	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = AccelY*(0.0039*9.8);
	if(Usart_handler != NULL){
		sprintf(data_Buffer, "Y axis data = %.2f\n", ValorY);
		interruptWriteMsg(Usart_handler, data_Buffer);
	}

	if(single_Data_archive != NULL){
		single_Data_archive->Y_Data[y_position] = ValorY;
	}

}

void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t z_position, char *data_Buffer){

	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = (float) AccelZ*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(data_Buffer, "Z axis data = %.2f\n", ValorZ);
		interruptWriteMsg(Usart_handler, data_Buffer);
	}

	if(single_Data_archive != NULL){
		single_Data_archive->Z_Data[z_position] = ValorZ;
	}
}

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler, char *data_Buffer){
	/* Lectura e impresión de registro ID */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, ACCEL_ID);
	sprintf(data_Buffer, "Who am I? (r) = %d\n", i2c_dataBuffer);
	interruptWriteMsg(Usart_handler, data_Buffer);
}

void read_PowerMode(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler, char *data_Buffer){
	/* Configurara a modo medida */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, POWER_CTL);

	/* Configuraciones encontradas posibles en el acelerador */
	if(i2c_dataBuffer == MEASURE_MODE){
		sprintf(data_Buffer, "AXL345 measuring\n");
		interruptWriteMsg(Usart_handler, data_Buffer);
	} else {
		sprintf(data_Buffer, "Not configured\n");
		interruptWriteMsg(Usart_handler, data_Buffer);
	}
}

void measure_Mode_config(I2C_Handler_t *accel_handler){
 	/* Configuración a modo de medida*/
	i2c_writeSingleRegister(accel_handler, POWER_CTL, MEASURE_MODE);
}

void constant_100Khz_measuring(I2C_Handler_t *accel_handler, BasicTimer_Handler_t *timer100Khz_handler,
								GPIO_Handler_t *pinTimer100khz){
	/* Configuración TIM4 para ser el control del muestreo a 1Khz*/
	timer100Khz_handler->ptrTIMx = TIM4;
	timer100Khz_handler->TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timer100Khz_handler->TIMx_Config.TIMx_speed = 1600;
	timer100Khz_handler->TIMx_Config.TIMx_period = 100; // 1 ms
	timer100Khz_handler->TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(timer100Khz_handler);
}

void XYZ_dataset(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *xyz_Data_Archive, uint8_t set_position, char *data_Buffer){
	/* Guarda las variables en la posición "set_position" y no las envía a través de
	 * USART individualmente */
	single_data_X(NULL, accel_handler, xyz_Data_Archive, set_position,data_Buffer);
	single_data_Y(NULL,	accel_handler, xyz_Data_Archive, set_position,data_Buffer);
	single_data_Z(NULL, accel_handler, xyz_Data_Archive, set_position,data_Buffer);

	float axisX_Value = xyz_Data_Archive->X_Data[set_position];
	float axisY_Value = xyz_Data_Archive->Y_Data[set_position];
	float axisZ_Value = xyz_Data_Archive->Z_Data[set_position];

	if(Usart_handler != NULL){
		sprintf(data_Buffer, "(%.2f ; %.2f ; %.2f)\n",
							axisX_Value, axisY_Value, axisZ_Value);
		interruptWriteMsg(Usart_handler, data_Buffer);
	}
}

void print_All_Data(USART_Handler_t *Usart_handler, axis_Data_t *xyz_Data_Archive,
					uint8_t print_position, char *data_Buffer){
	do {
		float axisX_Value = xyz_Data_Archive->X_Data[print_position];
		float axisY_Value = xyz_Data_Archive->Y_Data[print_position];
		float axisZ_Value = xyz_Data_Archive->Z_Data[print_position];

		sprintf(data_Buffer, "Axis data\n(x ; y ; z)\n(%.2f ; %.2f ; %.2f)\n", axisX_Value, axisY_Value, axisZ_Value);
		interruptWriteMsg(Usart_handler, data_Buffer);

		print_position++;

	} while(print_position != BIGDATA_BUFFER);
}

float raw_data_X(I2C_Handler_t *accel_handler){
	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = (float) AccelX*(0.0039*9.8);

	return ValorX;
}

float raw_data_Y(I2C_Handler_t *accel_handler){
	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = AccelY*(0.0039*9.8);

	return ValorY;
}

float raw_data_Z(I2C_Handler_t *accel_handler){
	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = AccelZ*(0.0039*9.8);

	return ValorZ;
}




























