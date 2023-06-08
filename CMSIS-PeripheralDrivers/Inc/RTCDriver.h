/*
 * RTCDriver.h
 *
 *  Created on: Jun 4, 2023
 *      Author: ingfisica
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include <stm32f4xx.h>

#define RTC_KEY1	0XCA
#define RTC_KEY2	0x53

#define PM 	1
#define AM	0

#define LUNES	 	1
#define MARTES		2
#define MIERCOLES	3
#define JUEVES		4
#define VIERNES 	5
#define SABADO		6
#define DOMINGO 	7

#define MONDAY	 	1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRYDAY 		5
#define SATURDAY	6
#define SUNDAY 		7

typedef struct
{
	uint8_t meridian;
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t year;
	uint8_t month;
	uint8_t date;
	char weekday[10];
} RTC_Data_t;


void RTC_config(void);
void RTC_Time_Change(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t meridian);
void RTC_Date_Change(uint8_t date, uint8_t month, uint8_t year,  uint8_t weekday);
void save_RTC_Data(RTC_Data_t *ptrRTC_DAta);
uint8_t binaryToBCD(uint8_t bin_Value);





#endif /* RTCDRIVER_H_ */
