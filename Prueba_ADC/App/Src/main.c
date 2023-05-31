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
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};

// Handler ADC
ADC_Config_t adc_handler = {0};
int16_t data_ADC = 0;

// Variables transmisión USART
GPIO_Handler_t PinTX_handler = {0};
GPIO_Handler_t PinRX_handler = {0};
USART_Handler_t USART2_handler = {0};
char data_recibida_USART2 = 0;
char datos[128] = "Usart";


//Funciones
void inicializacion_Led_Estado(void);
void inicializacion_ADC_CH0(void);
void inicializacion_pines_USART(void);


int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	inicializacion_Led_Estado();
	inicializacion_pines_USART();
	inicializacion_ADC_CH0();

	interruptWriteMsg(&USART2_handler, datos);

    /* Loop forever */
	while(1){
		if(data_recibida_USART2 == 's'){
			startSingleADC();
			sprintf(datos, "%d\n", data_ADC);
			interruptWriteMsg(&USART2_handler, datos);
			data_recibida_USART2 = '\0';

		}

		else if(data_recibida_USART2 == 'c'){
			startContinousADC();
			data_recibida_USART2 = '\0';
		}
	}

	return 0;
}


void callback_USART2_RX(void){
	data_recibida_USART2 = get_data_RX();
}

void adcComplete_Callback(void){
	data_ADC = getADC();
}

void inicializacion_ADC_CH0(void){
	adc_handler.channel = ADC_CHANNEL_0;
	adc_handler.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adc_handler.resolution = ADC_RESOLUTION_10_BIT;
	adc_handler.samplingPeriod = ADC_SAMPLING_PERIOD_15_CYCLES;
	adc_Config(&adc_handler);
}


/* Pin A5 */
void inicializacion_Led_Estado(void){
	// Timer para LED de estado usando el LED2
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
}


void inicializacion_pines_USART(void){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART2
	PinTX_handler.pGPIOx = GPIOA;
	PinTX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinTX_handler);

	// Inicialización de PIN A3 con función alternativa de USART2
	PinRX_handler.pGPIOx = GPIOA;
	PinRX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinRX_handler);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	USART2_handler.ptrUSARTx = USART2;
	USART2_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART2_handler.USART_Config.USART_baudrate = USART_BAUDRATE_9600;
	USART2_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART2_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART2_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART2_handler.USART_Config.USART_enableIntRX = ENABLE;
	USART2_handler.USART_Config.USART_enableIntTX = ENABLE;
	USART2_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART2_handler);
}
