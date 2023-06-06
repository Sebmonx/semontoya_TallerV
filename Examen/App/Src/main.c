/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoyas
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
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "RTCDriver.h"
#include "funciones.h"

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH1 = {0};

// Variables transmisión USART
GPIO_Handler_t PinTX2_h = {0};
GPIO_Handler_t PinRX2_h = {0};

GPIO_Handler_t PinTX6_h = {0};
GPIO_Handler_t PinRX6_h = {0};

USART_Handler_t USART_h = {0};

// Variables PLL
PLL_Config_t PLL_h = {0};
GPIO_Handler_t MCO1_h = {0};

int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);
	systemClock_100MHz(&PLL_h);
	inicializacion_Led_Estado(&timerLed, &blinkyLed, &blinkyLedH1);
	inicializacion_USART2(&PinTX2_h, &PinRX2_h, &USART_h);
    /* Loop forever */
	while(1){

	}

	return 0;
}



void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	GPIOxTooglePin(&blinkyLedH1);
}
