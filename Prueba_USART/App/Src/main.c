/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
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
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"

GPIO_Handler_t PinTX_handler = {0};
USART_Handler_t USART2_TX_handler = {0};

BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};


void inicializacion_pines(void);


int main(void)
{
	char dataToSend = 'd';
	inicializacion_pines();
    /* Loop forever */
	while(1){
		for(uint8_t i = 0; i < 5; i++){
			writeChar(&USART2_TX_handler, dataToSend);
		}
	}

	return 0;
}

void inicializacion_pines(void){
	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa asociada al USART
	PinTX_handler.pGPIOx = GPIOA;
	PinTX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	USART2_TX_handler.ptrUSARTx = USART2;
	USART2_TX_handler.USART_Config.USART_mode = USART_MODE_TX;
	USART2_TX_handler.USART_Config.USART_baudrate = USART_BAUDRATE_9600;
	USART2_TX_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART2_TX_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART2_TX_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;

}

void inicializacion_led_estado(void){
	// Timer para LED de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerLed.TIMx_Config.TIMx_period = 300; // Tiempo en milisegundos
	timerLed.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
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

/* Centelleo de LED de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
}