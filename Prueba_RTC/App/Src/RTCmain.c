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

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH0 = {0};

// Handler RTC
RTC_Data_t RTC_Data = {0};

// Variables transmisión USART
GPIO_Handler_t PinTX2_handler = {0};
GPIO_Handler_t PinRX2_handler = {0};

GPIO_Handler_t PinTX6_handler = {0};
GPIO_Handler_t PinRX6_handler = {0};

USART_Handler_t USART_handler = {0};

char data_recibida_USART = 0;
char buffer_datos[256] = "Test\n";
char buffer_Recepcion[256] = {0};
uint8_t contador_recepcion = 0;
bool string_completada = false;

void inicializacion_Led_Estado(void);
void inicializacion_USART2(void);

int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	RTC_config();
	inicializacion_Led_Estado();
	inicializacion_USART2();
//	interruptWriteMsg(&USART_handler, buffer_datos);
	writeMsg(&USART_handler, buffer_datos);


    /* Loop forever */
	while(1){
		if(data_recibida_USART != '\0'){
			if(data_recibida_USART == 'a'){
				save_RTC_Data(&RTC_Data);
				sprintf(buffer_datos,"%d : %d\n", RTC_Data.hour, RTC_Data.seconds);
				interruptWriteMsg(&USART_handler, buffer_datos);
			}

			else if (data_recibida_USART == 'i') {
//				save_RTC_Data(&RTC_Data);
//				sprintf(buffer_datos, "%d : %d\n", RTC_Data.hour,
//						RTC_Data.seconds);
				writeMsg(&USART_handler, "Prueba de sonido!");
			}
			data_recibida_USART = '\0';
		}

	}

	return 0;
}


void BasicTimer2_Callback(){
	GPIOxTooglePin(&blinkyLed);
}


//void callback_USART2_RX(void){
//	data_recibida_USART = get_data_RX();
//}

void callback_USART6_RX(void){
	data_recibida_USART = get_data_RX();
}

/* Pin A5 y pin H0 */
void inicializacion_Led_Estado(void){

	RCC->CR &= ~RCC_CR_HSEON;

	// Timer para LEDs de estado usando el LED2 y pin H0
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BITMER_SPEED_16Mhz_100us;
	timerLed.TIMx_Config.TIMx_period = 2500; 			// Tiempo en milisegundos
	timerLed.TIMx_Config.TIMx_interruptEnable = ENABLE; // Activar interrupción
	BasicTimer_Config(&timerLed);

	// Controlador de LED2 asignado como led de estado
	blinkyLed.pGPIOx = GPIOA;
	blinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	blinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	blinkyLed.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&blinkyLed);

	// Controlador de pinH0 asignado como led de estado
	blinkyLedH0.pGPIOx = GPIOH;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&blinkyLedH0);
}

void inicializacion_USART2(void){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART2
	PinTX2_handler.pGPIOx = GPIOA;
	//PinTX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	//PinTX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinTX2_handler);

	// Inicialización de PIN A3 con función alternativa de USART2
	PinRX2_handler.pGPIOx = GPIOA;
	//PinRX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	//PinRX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinRX2_handler);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	//USART_handler.ptrUSARTx = USART2;
	USART_handler.ptrUSARTx = USART6;
	USART_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_handler.USART_Config.USART_baudrate = USART_BAUDRATE_9600;
	USART_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_handler.USART_Config.USART_enableIntRX = ENABLE;
	USART_handler.USART_Config.USART_enableIntTX = DISABLE;
	USART_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART_handler);
}
