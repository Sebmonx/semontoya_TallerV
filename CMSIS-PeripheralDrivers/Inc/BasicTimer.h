/*
 * BasicTimer.h
 *
 *  Created on: Apr 18, 2022
 *      Author: Sebastian Montoya
 */

#ifndef BASICTIMER_H_
#define BASICTIMER_H_

#include "stm32f4xx.h"

#define BTIMER_MODE_UP		0
#define BTIMER_MODE_DOWN	1



#define BTIMER_SPEED_80Mhz_100us	8100

#define BTIMER_SPEED_100Mhz_100us	1000
#define BTIMER_SPEED_100Mhz_10us	100
#define BTIMER_SPEED_100Mhz_1us		10
#define BTIMER_SPEED_100Mhz_100ns	1

#define BTIMER_SPEED_16Mhz_10us 	160
#define BTIMER_SPEED_16Mhz_100us	1600
#define BTIMER_SPEED_16Mhz_1ms		16000

#define BTIMER_INTERRUPT_DISABLE 	0
#define BTIMER_INTERRUPT_ENABLE 	1



/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 				// Up or dowm
	uint32_t	TIMx_speed;				// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;			// Valor del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}BasicTimer_Config_t;


/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;
}BasicTimer_Handler_t;


/* Configuración para timer input capture */
typedef struct
{
	uint8_t		TIMx_mode; 				// Up or dowm
	uint32_t	TIMx_speed;				// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;			// Valor del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}ATimer_Config_t;

/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	ATimer_Config_t		TIMx_Config;
}ATimer_Handler_t;

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimer2_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer3_Callback(void);
void BasicTimer4_Callback(void);
void BasicTimer5_Callback(void);

void timer1_config(BasicTimer_Handler_t *ptrBTimerHandler);


#endif /* BASICTIMER_H_ */
