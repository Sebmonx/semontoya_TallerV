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
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

/* Handlers */
// PWM
GPIO_Handler_t pinPWM_handler ={0};
PWM_Handler_t PWM_handler = {0};

// USART6
GPIO_Handler_t PinTX_handler = {0};
GPIO_Handler_t PinRX_handler = {0};
USART_Handler_t USART2_handler = {0};

// LED de estado
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};

// PLL
PLL_Config_t config_PLL = {0};
system_Clock_data data_Reloj = {0};

/* ######## */


/* Variables */
char data_recibida_USART6 = 0;

/* ######### */


/* Funciones inicialización */
void inicializacion_Led_Estado_80Mhz(void);
void inicializacion_pines_USART(void);
void inicializacion_PWM(void);


/* ######################## */


/* Otras funciones */
/* ############### */




int main(void)
{
	/* Activador coprocesador matemático - FPU */
	//SCB->CPACR |= (0xF << 20);

    /* Loop forever */
	while(1){

	}

	return 0;
}

/* Pin A5 y TIM2 */
void inicializacion_Led_Estado_80Mhz(void){
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

/* Pines A2 y A3 */
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
	USART2_handler.USART_Config.USART_enableIntRX = USART_INTERRUPT_RX_ENABLE;
	USART2_handler.USART_Config.USART_enableIntTX = USART_INTERRUPT_TX_NONE;
	USART_Config(&USART2_handler);
}

/* Temporalmente PIN B5 y TIM3 CH2 */
void inicializacion_PWM(void){
	pinPWM_handler.pGPIOx = GPIOB;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWM_handler);

	PWM_handler.ptrTIMx = TIM3;
	PWM_handler.config.channel = PWM_CHANNEL_2;
	PWM_handler.config.periodo = 20000;
	PWM_handler.config.prescaler = 16; /* Incremento cada microsegundo */
	PWM_handler.config.duttyCicle = 0;
	pwm_Config(&PWM_handler);

}

/* Centelleo led de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
}

/* Interrupción por recepción USART */
void callback_USART6_RX(void){
	data_recibida_USART6 = get_data_RX();
}
