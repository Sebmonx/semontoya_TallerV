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
#include "GPIOxDriver.h"


#define AXL345_ADDRESS	0x1D //Acelerometro AXL345
#define ACCEL_XOUT_L	0x32
#define ACCEL_XOUT_H	0x33
#define ACCEL_YOUT_L 	0x34
#define ACCEL_YOUT_H 	0x35
#define ACCEL_ZOUT_L 	0x36
#define ACCEL_ZOUT_H 	0x37

#define POWER_CTL 		45
#define MEASURE_MODE	0x2D
#define ACCEL_ID  		0

#define DATA_BUFFER		64
#define BIGDATA_BUFFER	2000

#define STANDARD_DATA		0
#define XYZ_CONSTANT_DATA 	1
#define XYZ_2SEC_DATA		2

typedef struct{
	float X_Data[BIGDATA_BUFFER];
	float Y_Data[BIGDATA_BUFFER];
	float Z_Data[BIGDATA_BUFFER];
}axis_Data_t;



void inicializacion_AXL345(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void single_data_X(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t x_position);
void single_data_Y(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t y_position);
void single_data_Z(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *single_Data_archive, uint16_t z_position);

void device_ID(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void read_PowerMode(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler);
void measure_Mode_config(I2C_Handler_t *accel_handler);
void constant_100Khz_measuring(I2C_Handler_t *accel_handler, BasicTimer_Handler_t *timer100Khz_handler,
								GPIO_Handler_t *pinTimer100khz);
void XYZ_dataset(USART_Handler_t *Usart_handler, I2C_Handler_t *accel_handler,
					axis_Data_t *xyz_Data_archive, uint16_t set_position);
void print_All_Data(USART_Handler_t *Usart_handler, axis_Data_t *xyz_Data_Archive, uint16_t print_position);
float raw_data_X(I2C_Handler_t *accel_handler);
float raw_data_Y(I2C_Handler_t *accel_handler);
float raw_data_Z(I2C_Handler_t *accel_handler);
void print_XYZ_Data(axis_Data_t *xyz_Data, USART_Handler_t *Usart_handler, uint16_t print_position);

void read_XYZ_data(I2C_Handler_t *accel_handler, axis_Data_t *axis_Data, uint8_t acel_Pos);
void read_Z_data(I2C_Handler_t *accel_handler, uint16_t *axis_Data, uint16_t acel_Pos);
#endif /* ACELEROMETRODRIVER_H_ */
