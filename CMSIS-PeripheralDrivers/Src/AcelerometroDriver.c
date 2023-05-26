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
char usart_dataBuffer[128] = "Accel MPU-6050 testing...";

void inicializacion_AXL345(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Mensaje de inicio */
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	/* Lectura e impresión de registro ID */
	device_ID(Usart_handler, accel_handler);

	/* Configuración a modo de medida*/
	strcpy(usart_dataBuffer, "Power control\n");
	interruptWriteMsg(Usart_handler, usart_dataBuffer);
	measure_Mode_config(accel_handler);

	/* Confirmación de configuración */
	read_PowerMode(Usart_handler, accel_handler);
}

void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t x_position){

	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	uint8_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = AccelX*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "X axis data = %f\n", ValorX);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	if(single_Data_archive != NULL){

		single_Data_archive->independent_X_Data[x_position] = ValorX;

		if(x_position == (sizeof(single_Data_archive)/sizeof(ValorX))-1){
				x_position = 0;
		}
	}
}

void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t y_position){

	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	uint8_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = AccelY*(0.0039*9.8);
	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Y axis data = %f\n", ValorY);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}


	if(single_Data_archive != NULL){

		single_Data_archive->independent_Y_Data[y_position] = ValorY;

		if(y_position == (sizeof(single_Data_archive)/sizeof(ValorY))-1){
				y_position = 0;
		}
	}

}

void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t z_position){

	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	uint8_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = AccelZ*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Z axis data = %f\n", ValorZ);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	if(single_Data_archive != NULL){

		single_Data_archive->independent_Z_Data[z_position] = ValorZ;

		if(z_position == (sizeof(single_Data_archive)/sizeof(ValorZ))-1){
				z_position = 0;
		}
	}
}

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Lectura e impresión de registro ID */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, ACCEL_ID);
	sprintf(usart_dataBuffer, "Who am I? (r) = %d\n", (unsigned int)i2c_dataBuffer);
	interruptWriteMsg(Usart_handler, usart_dataBuffer);
}

void read_PowerMode(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Configurara a modo medida */
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
	i2c_writeSingleRegister(accel_handler, POWER_CTL, MEASURE_MODE);
}

void constant_100Khz_measuring(I2C_Handler_t *accel_handler, BasicTimer_Handler_t *timer100Khz_handler,
								uint8_t data_recieve_mode){
	/* Variable que controla la forma de recibir datos */
	data_recieve_mode = XYZ_CONSTANT_DATA;

	timer100Khz_handler->ptrTIMx = TIM3;
	timer100Khz_handler->TIMx_Config.TIMx_mode = BTIMER_MODE_DOWN;
	timer100Khz_handler->TIMx_Config.TIMx_speed = BITMER_SPEED_16Mhz_1ms;
	timer100Khz_handler->TIMx_Config.TIMx_period = 1; // 1 ms
	timer100Khz_handler->TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(timer100Khz_handler);

	strcpy(usart_dataBuffer, "1Khz Continous measure\n");

}

void XYZ_dataset(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *xyz_Data_archive, uint8_t set_position){
	/* Guarda las variables en la posición "set_position" y no las envía a través de
	 * USART individualmente */
	single_data_X(NULL, accel_handler, xyz_Data_archive, set_position);
	single_data_Y(NULL,	accel_handler, xyz_Data_archive, set_position);
	single_data_Z(NULL, accel_handler, xyz_Data_archive, set_position);

	float axisX_Value = xyz_Data_archive->independent_X_Data[set_position];
	float axisY_Value = xyz_Data_archive->independent_Y_Data[set_position];
	float axisZ_Value = xyz_Data_archive->independent_Z_Data[set_position];

	if(Usart_handler != NULL && strcmp(usart_dataBuffer, "3 Axis data\n")){
		strcpy(usart_dataBuffer, "3 Axis data\n");
		interruptWriteMsg(Usart_handler, usart_dataBuffer);

	} else if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "X data = %.2f \t Y data = %.2f \t Z data = %.2f\n",
					axisX_Value, axisY_Value, axisZ_Value);

		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}
}

































