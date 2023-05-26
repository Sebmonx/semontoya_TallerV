/*
 * AcelerometroDriver.h
 *
 *  Created on: May 25, 2023
 *      Author: ingfisica
 */

#ifndef ACELEROMETRODRIVER_H_
#define ACELEROMETRODRIVER_H_

#include "stm32f4xx.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "BasicTimer.h"


#define AXL345_ADDRESS	0x1D //Acelerometro AXL345
#define ACCEL_XOUT_L	50
#define ACCEL_XOUT_H	51
#define ACCEL_YOUT_L 	52
#define ACCEL_YOUT_H 	53
#define ACCEL_ZOUT_L 	54
#define ACCEL_ZOUT_H 	55

#define POWER_CTL 		45
#define MEASURE_MODE	0x2D
#define ACCEL_ID  		0

#define DATA_BUFFER		64
#define BIGDATA_BUFFER	6500

#define STANDARD_DATA		0
#define XYZ_CONSTANT_DATA 	1

typedef struct{
	float independent_X_Data[BIGDATA_BUFFER];
	float independent_Y_Data[BIGDATA_BUFFER];
	float independent_Z_Data[BIGDATA_BUFFER];
}axis_Data_t;



void inicializacion_AXL345(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t x_count);
void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t y_count);
void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint8_t z_count);

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void read_PowerMode(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void measure_Mode_config(I2C_Handler_t *accel_handler);

#endif /* ACELEROMETRODRIVER_H_ */
