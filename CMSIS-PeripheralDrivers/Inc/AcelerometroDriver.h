/*
 * AcelerometroDriver.h
 *
 *  Created on: May 25, 2023
 *      Author: ingfisica
 */

#ifndef ACELEROMETRODRIVER_H_
#define ACELEROMETRODRIVER_H_

#include "stm32f4xx.h"

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
#define BIGDATA_BUFFER	6002


typedef struct{
	float independent_X_Data[DATA_BUFFER];
	float independent_Y_Data[DATA_BUFFER];
	float independent_Z_Data[DATA_BUFFER];
}axis_Data_t;

typedef struct{
	float big_X_data[BIGDATA_BUFFER];
	float big_Y_data[BIGDATA_BUFFER];
	float big_Z_data[BIGDATA_BUFFER];
}axis_BigData_t;


#endif /* ACELEROMETRODRIVER_H_ */
