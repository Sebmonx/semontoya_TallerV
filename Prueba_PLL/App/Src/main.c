/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "PLLDriver.h"
#include "BasicTimer.h"

// Handler GPIO A8
GPIO_Handler_t MCO1 = {0};
GPIO_Handler_t MCO2 = {0};

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};

// Handlers para PLL
PLL_Config_t config_PLL = {0};
system_Clock_data data_Reloj = {0};

// Funciones a utilizar
void inicializacion_Led_Estado(void);


int main(void)
{
	systemClock_80MHz(&config_PLL);
	inicializacion_Led_Estado();
	systemClock_GetConfig(&data_Reloj);

	/* Loop forever */
	while(1){
		__NOP();
	}

	return 0;
}


void inicializacion_Led_Estado(void){
	// Timer para LED de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = 1000;
	timerLed.TIMx_Config.TIMx_period = 1000; // Tiempo en milisegundos
	timerLed.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	timerLed.TIMx_Config.APB1_frequency = 40; // Frecuencia en bus conectado a Timer
	BasicTimer_Config(&timerLed);

	// Controlador de LED2 asignado como led de estado
	blinkyLed.pGPIOx = GPIOA;
	blinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	blinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	blinkyLed.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&blinkyLed);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
}
