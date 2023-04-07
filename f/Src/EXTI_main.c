/*
 * EXtiConfig_Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: ingfisica
 */


#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/* Definicion de los elementos */

GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserB = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

uint32_t counterExti13 = 0;

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_exti13(void);


int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();


	while(1){

	}


	return 0;
}


void init_Hardware(void){

	/* Config TIM2 para encender cada 300 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 300; // Interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = 1;//BITMER_INTERRUP_ENALBE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Config Led2 - PA5 */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 	= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);
//	GPIO_WritePin(&handlerLED2, SET);


}



void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}
