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
							BasicTimer_Handler_t *timer100Khz_handler, GPIO_Handler_t *pinTimer100khz){
	/* Mensaje de inicio */
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	/* Lectura e impresión de registro ID */
	device_ID(Usart_handler, accel_handler);

	/* Configuración a modo de medida*/
	sprintf(usart_dataBuffer, "Power control\n");
	interruptWriteMsg(Usart_handler, usart_dataBuffer);
	measure_Mode_config(accel_handler);
	//constant_100Khz_measuring(accel_handler, timer100Khz_handler, pinTimer100khz);
	/* Confirmación de configuración */
	read_PowerMode(Usart_handler, accel_handler);
}

void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t x_position){

	uint8_t AccelX_Low = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_L);
	uint8_t AccelX_High = i2c_readSingleRegister(accel_handler, ACCEL_XOUT_H);
	uint16_t AccelX = AccelX_High << 8 | AccelX_Low;
	float ValorX = (float) AccelX*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "X axis data = %.2f\n", ValorX);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	if(single_Data_archive != NULL){

		single_Data_archive->X_Data[x_position] = ValorX;

		if(x_position == (sizeof(single_Data_archive)/sizeof(ValorX))){
				x_position = 0;
		} else {
			x_position++;
		}
	}
}

void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t y_position){

	uint8_t AccelY_Low = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_L);
	uint8_t AccelY_High = i2c_readSingleRegister(accel_handler, ACCEL_YOUT_H);
	uint16_t AccelY = AccelY_High << 8 | AccelY_Low;
	float ValorY = AccelY*(0.0039*9.8);
	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Y axis data = %f\n", ValorY);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}


	if(single_Data_archive != NULL){

		single_Data_archive->Y_Data[y_position] = ValorY;

		if(y_position == (sizeof(single_Data_archive)/sizeof(ValorY))){
				y_position = 0;
		} else {
			y_position++;
		}
	}

}

void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t z_position){

	uint8_t AccelZ_Low = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_L);
	uint8_t AccelZ_High = i2c_readSingleRegister(accel_handler, ACCEL_ZOUT_H);
	uint16_t AccelZ = AccelZ_High << 8 | AccelZ_Low;
	float ValorZ = (float) AccelZ*(0.0039*9.8);

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "Z axis data = %f\n", ValorZ);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}

	if(single_Data_archive != NULL){

		single_Data_archive->Z_Data[z_position] = ValorZ;

		if(z_position == (sizeof(single_Data_archive)/sizeof(ValorZ))){
				z_position = 0;
		} else {
			z_position++;
		}
	}
}

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler){
	/* Lectura e impresión de registro ID */
	i2c_dataBuffer = i2c_readSingleRegister(accel_handler, ACCEL_ID);
	sprintf(usart_dataBuffer, "Who am I? (r) = %d\n", i2c_dataBuffer);
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
					axis_Data_t *xyz_Data_Archive, uint8_t set_position){
	/* Guarda las variables en la posición "set_position" y no las envía a través de
	 * USART individualmente */
	single_data_X(NULL, accel_handler, xyz_Data_Archive, set_position);
	single_data_Y(NULL,	accel_handler, xyz_Data_Archive, set_position);
	single_data_Z(NULL, accel_handler, xyz_Data_Archive, set_position);

	float axisX_Value = xyz_Data_Archive->X_Data[set_position];
	float axisY_Value = xyz_Data_Archive->Y_Data[set_position];
	float axisZ_Value = xyz_Data_Archive->Z_Data[set_position];

	set_position++;

	if(Usart_handler != NULL){
		sprintf(usart_dataBuffer, "(%.2f ; %.2f ; %.2f)\n",
							axisX_Value, axisY_Value, axisZ_Value);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);
	}
}

void print_All_Data(USART_Handler_t *Usart_handler, axis_Data_t *xyz_Data_Archive, uint8_t print_position){
	print_position = 0;
	sprintf(usart_dataBuffer, "3 Axis data\n(x ; y ; z)\n");
	interruptWriteMsg(Usart_handler, usart_dataBuffer);

	do {
		float axisX_Value = xyz_Data_Archive->X_Data[print_position];
		float axisY_Value = xyz_Data_Archive->Y_Data[print_position];
		float axisZ_Value = xyz_Data_Archive->Z_Data[print_position];

		sprintf(usart_dataBuffer, "(%.2f ; %.2f ; %.2f)\n", axisX_Value, axisY_Value, axisZ_Value);
		interruptWriteMsg(Usart_handler, usart_dataBuffer);

		if(print_position == (sizeof(xyz_Data_Archive)/sizeof(float))){
			print_position = 0;
		}
		print_position++;

	} while(xyz_Data_Archive->Z_Data[print_position] != 0);
}
































