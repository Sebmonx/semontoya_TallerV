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

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH0 = {0};

// Handler ADC
ADC_Config_t adc_handler = {0};
uint16_t ADC_Data[2] = {0};
uint8_t ADC_Contador = 0;
uint8_t numero_De_Canales = 0;
uint8_t ADC_Completo = 0;

// Variables transmisión USART
GPIO_Handler_t PinTX2_handler = {0};
GPIO_Handler_t PinRX2_handler = {0};

GPIO_Handler_t PinTX6_handler = {0};
GPIO_Handler_t PinRX6_handler = {0};

USART_Handler_t USART_handler = {0};

char data_recibida_USART = 0;
char buffer_datos[128] = "Test\n";
char buffer_Recepcion[256] = {0};
uint8_t contador_recepcion = 0;
bool string_completada = false;

// Variables de comandos
char cmd[64] = {0};
char userMsg[256] = {0};
unsigned int parametro_1 = 0;
unsigned int parametro_2 = 0;


//Funciones
void inicializacion_Led_Estado(void);
void inicializacion_ADC_CH0(void);
void inicializacion_USART2(void);
void inicializacion_USART6(void);
void chequear_Comando(char *ptrBuffer_Recepcion);

int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	inicializacion_Led_Estado();
	inicializacion_USART2();
	inicializacion_ADC_CH0();
	GPIO_WritePin(&blinkyLedH0, ENABLE);
	interruptWriteMsg(&USART_handler, buffer_datos);

    /* Loop forever */
	while(1){
		if(data_recibida_USART != '\0'){
			if(data_recibida_USART == 'a'){
				startSingleADC();
				startSingleADC();
				sprintf(buffer_datos,"%d , %d", ADC_Data[0], ADC_Data[1]);
				interruptWriteMsg(&USART_handler, buffer_datos);
				data_recibida_USART = '\0';
			}
		}
	}
	return 0;
}


void chequear_Comando(char *ptrBuffer_Recepcion){
	sscanf(ptrBuffer_Recepcion,"%s %u %u %s", cmd, &parametro_1, &parametro_2, userMsg);

	if(strcmp(cmd, "help") == 0){
		imprimir
	}
	else if(strcmp(cmd, "test") == 0){

	}
	else {
		interruptWriteMsg(&USART_handler, "Comando erróneo.");
	}

}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	GPIOxTooglePin(&blinkyLedH0);
}

void callback_USART2_RX(void){
	data_recibida_USART = get_data_RX();
}

void adcComplete_Callback(void){
	ADC_Data[ADC_Contador] = getADC();
	ADC_Contador++;
	if(ADC_Contador == numero_De_Canales){
		ADC_Contador = 0;
		ADC_Completo = 1;
	}
}


void test_cmd(uint8_t parametro1, uint8_t parametro2, USART_Handler_t *ptrUSART){
	interruptWriteMsg(&USART_handler, "Test CMD\n");

	sprintf(buffer_datos, "numero 1 = %u", parametro1);
	interruptWriteMsg(&USART_handler, buffer_datos);

	sprintf(buffer_datos, "numero 2 = %u", parametro2);
	interruptWriteMsg(&USART_handler, buffer_datos);
}

void imprimir_help(USART_Handler_t *ptrUSART){
	interruptWriteMsg(&USART_handler, "Menú de comandos\n");
	interruptWriteMsg(&USART_handler, "1) help - Imprimir este menú\n");
	interruptWriteMsg(&USART_handler, "2) - Control 1 de MCO1 cambiar señal - Elige entre HSI, LSE o PLL\n");
	interruptWriteMsg(&USART_handler, "3) - Control 2 de MCO2 cambiar preescaler - Elige entre \n");
	interruptWriteMsg(&USART_handler, "4) - RTC 1\n");
	interruptWriteMsg(&USART_handler, "5) - RTC 2\n");
	interruptWriteMsg(&USART_handler, "6) - RTC 3\n");
	interruptWriteMsg(&USART_handler, "7) - RTC 4\n");
	interruptWriteMsg(&USART_handler, "8) - ADC 1\n");
	interruptWriteMsg(&USART_handler, "9) - ADC 2\n");
	interruptWriteMsg(&USART_handler, "10) - Acel 1\n");
	interruptWriteMsg(&USART_handler, "11) - Acel 2\n");
}

void inicializacion_ADC_CH0(void){
	adc_handler.channel[0] = ADC_CHANNEL_0;
	adc_handler.channel[1] = ADC_CHANNEL_1;
	adc_handler.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adc_handler.resolution = ADC_RESOLUTION_10_BIT;
	adc_handler.samplingPeriod = ADC_SAMPLING_PERIOD_15_CYCLES;
	numero_De_Canales = 2;
	adc_Config_MultiCH(&adc_handler, numero_De_Canales);

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
	PinTX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinTX2_handler);

	// Inicialización de PIN A3 con función alternativa de USART2
	PinRX2_handler.pGPIOx = GPIOA;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinRX2_handler);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	USART_handler.ptrUSARTx = USART2;
	USART_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_handler.USART_Config.USART_baudrate = USART_BAUDRATE_9600;
	USART_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_handler.USART_Config.USART_enableIntRX = ENABLE;
	USART_handler.USART_Config.USART_enableIntTX = ENABLE;
	USART_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART_handler);
}

void inicializacion_USART6(void){

	// Inicializacion de PIN A11 con funcion alternativa de USART
	PinTX6_handler.pGPIOx = GPIOA;
	PinTX6_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	PinTX6_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX6_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinTX6_handler);

	// Inicialización de PIN A12 con función alternativa de USART
	PinRX6_handler.pGPIOx = GPIOA;
	PinRX6_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	PinRX6_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX6_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinRX6_handler);

	// Inicialización de módulo serial USART transmisión + recepción e interrupción RXTX
	USART_handler.ptrUSARTx = USART6;
	USART_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_handler.USART_Config.USART_baudrate = USART_BAUDRATE_19200;
	USART_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_handler.USART_Config.USART_enableIntRX = ENABLE;
	USART_handler.USART_Config.USART_enableIntTX = ENABLE;
	USART_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART_handler);
}
