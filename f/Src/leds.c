/*
 * leds.c
 *
 *  Created on: Apr 6, 2023
 *      Author: ingfisica
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "Display.h"
#include "Encoder.h"


/* Variables Display */
	BasicTimer_Handler_t timerDisplay = {0};

	/* Handlers de GPIO display */
	GPIO_Handler_t handPA8 = {0};
	GPIO_Handler_t handPB10 = {0};
	GPIO_Handler_t handPC4 = {0};
	GPIO_Handler_t handPB5 = {0};
	GPIO_Handler_t handPB3 = {0};
	GPIO_Handler_t handPA7 = {0};
	GPIO_Handler_t handPA9 = {0};

	/* Arreglo para encendido de LEDs 7 segmentos */
	GPIO_Handler_t bits_7Seg[7] = {0};

	/* Arreglo para encendido de culebrita */
	GPIO_Handler_t pos_snake[12] = {0};

	/* Handlers GPIO transistores */
	GPIO_Handler_t tranDec = {0};
	GPIO_Handler_t tranUni = {0};

/* Variables main */
	BasicTimer_Handler_t timerLED2 = {0};
	GPIO_Handler_t handLED2 = {0};
	uint8_t numero = 0;
	uint8_t decena = 0;
	uint8_t unidad = 0;
	uint8_t snake = 0;
	uint8_t modo = 0;

/* Variables Encoder */
	EXTI_Config_t encodCLK = {0};		// Pin para clock de encoder
	GPIO_Handler_t encodDT = {0};		// Pin para señal alternativa encoder
	EXTI_Config_t encodSW = {0}; 		// Pin para botón de cambio de función


void init_Display(void);
void init_Hardware(void);




int main(void){

	init_Display();
	init_Hardware();

	while(1){

	}
	return 0;
}


/* Inicialización de los pines a utilizar en el display */
void init_Display(void){

	/* LEDS */
	handPA8.pGPIOx = GPIOA;
	handPA8.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handPA8.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPA8.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPA8.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPA8.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPA8);
	GPIO_WritePin(&handPA8, SET);

	handPB10.pGPIOx = GPIOB;
	handPB10.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handPB10.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handPB10.GPIO_PinConfig.GPIO_PinOType 			= GPIO_OTYPE_PUSHPULL;
	handPB10.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB10.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB10);
	GPIO_WritePin(&handPB10, SET);

	handPC4.pGPIOx = GPIOC;
	handPC4.GPIO_PinConfig.GPIO_PinNumber 		= PIN_4;
	handPC4.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPC4.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPC4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPC4.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPC4);
	GPIO_WritePin(&handPC4, SET);

	handPB5.pGPIOx = GPIOB;
	handPB5.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handPB5.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPB5.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPB5.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB5.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB5);
	GPIO_WritePin(&handPB5, SET);

	handPB3.pGPIOx = GPIOB;
	handPB3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handPB3.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPB3.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPB3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB3.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB3);
	GPIO_WritePin(&handPB3, SET);

	handPA7.pGPIOx = GPIOA;
	handPA7.GPIO_PinConfig.GPIO_PinNumber 			= PIN_7;
	handPA7.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handPA7.GPIO_PinConfig.GPIO_PinOType 			= GPIO_OTYPE_PUSHPULL;
	handPA7.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPA7.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPA7);
	GPIO_WritePin(&handPA7, SET);

	handPA9.pGPIOx = GPIOA;
	handPA9.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handPA9.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPA9.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPA9.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPA9.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPA9);
	GPIO_WritePin(&handPA9, SET);

	/* Timer */
	timerDisplay.ptrTIMx = TIM3;
	timerDisplay.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	timerDisplay.TIMx_Config.TIMx_period			= 10; // Tiempo en milisegundos
	timerDisplay.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	timerDisplay.TIMx_Config.TIMx_interruptEnable 	= 1; // Activar interrupción
	BasicTimer_Config(&timerDisplay);

	/* Transistores */
	tranUni.pGPIOx = GPIOA;
	tranUni.GPIO_PinConfig.GPIO_PinNumber 		= PIN_10;
	tranUni.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	tranUni.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	tranUni.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	tranUni.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&tranUni);
	GPIO_WritePin(&tranUni, SET);

	tranDec.pGPIOx = GPIOA;
	tranDec.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	tranDec.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	tranDec.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	tranDec.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	tranDec.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&tranDec);
	GPIO_WritePin(&tranDec, RESET);

	/* Definicion de posiciones en arreglo para 7 segmentos*/
	bits_7Seg[0] = handPB3;
	bits_7Seg[1] = handPC4;
	bits_7Seg[2] = handPB10;
	bits_7Seg[3] = handPA8;
	bits_7Seg[4] = handPA9;
	bits_7Seg[5] = handPA7;
	bits_7Seg[6] = handPB5;

	/* Modificacion de variables */
	decena = numero/10;
	unidad = numero%10;
}


/* Inicializacion de hardware micro */
void init_Hardware(void){

	// Timer para LED de estado usando el LED2
	timerLED2.ptrTIMx = TIM2;
	timerLED2.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLED2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerLED2.TIMx_Config.TIMx_period = 300; // Tiempo en milisegundos
	timerLED2.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	BasicTimer_Config(&timerLED2);

	// Controlador de LED2 asignado para el estado de la función
	handLED2.pGPIOx = GPIOA;
	handLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handLED2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handLED2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handLED2.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handLED2);
}


/* Inicialización de los pines asociados a las señales entregadas
 * por el encoder */
void init_Encoder(void){

	// Señal del encoder que dirigirá la interrupción EXTI11
	encodCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	encodCLK.pGPIOHandler->pGPIOx = GPIOB;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	extInt_Config(&encodCLK);

	// Señal del encoder que determinará la dirección del movimiento
	encodDT.pGPIOx = GPIOB;
	encodDT.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	encodDT.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&encodDT);

	// Señal de encoder que lidera la interrupción EXTI12 de cambio de función
	encodSW.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	encodSW.pGPIOHandler->pGPIOx = GPIOB;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber = PIN_15;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	extInt_Config(&encodSW);

}


/* Centelleo de LED de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handLED2);
}


/* Alternar encendido de transistores */
void BasicTimer3_Callback(void){

	if(GPIO_ReadPin(&tranDec)){
		unidad = numero%10;
		numero_Display(unidad, bits_7Seg);
		GPIOxTooglePin(&tranUni);
		GPIOxTooglePin(&tranDec);
	}
	else{
		decena = numero/10;
		numero_Display(decena, bits_7Seg);
		GPIOxTooglePin(&tranDec);
		GPIOxTooglePin(&tranUni);
	}
}


/* Incremento o decremento por encoder */
void callback_extInt2(void){
	giro_Numero(numero, encodDT);
}


/* Modo de funcion en programa*/
void callback_extInt15(void){
	cambio_modo(modo);
}






