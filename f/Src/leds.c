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
	GPIO_Handler_t bit0 = {0};
	GPIO_Handler_t bit1 = {0};
	GPIO_Handler_t bit2 = {0};
	GPIO_Handler_t bit3 = {0};
	GPIO_Handler_t bit4 = {0};
	GPIO_Handler_t bit5 = {0};
	GPIO_Handler_t bit6 = {0};

	/* Arreglo para encendido de LEDs 7 segmentos */
	GPIO_Handler_t bits_7Seg[7] = {0};

	/* Arreglo para encendido de culebrita */
	GPIO_Handler_t pos_snake[12] = {0};

	/* Handlers GPIO transistores */
	GPIO_Handler_t tranDec = {0};
	GPIO_Handler_t tranUni = {0};

/* Variables genericas */
	BasicTimer_Handler_t timerLED2 = {0};
	GPIO_Handler_t handLED2 = {0};
	uint8_t numero = 0;
	uint8_t decena = 0;
	uint8_t unidad = 0;
	uint8_t snake = 0;
	uint8_t modo = 0;

/* Variables Encoder */
	GPIO_Handler_t encodCLK = {0};		// Pin para clock de encoder
	EXTI_Config_t exti_encodCLK = {0};	// Estructura para EXTI del clock de encoder
	GPIO_Handler_t encodDT = {0};		// Pin para señal alternativa encoder
	GPIO_Handler_t encodSW = {0}; 		// Pin para botón de cambio de función
	EXTI_Config_t exti_encodSW = {0}; 	// Estructura para EXTI de botón switch


void init_Hardware(void);



int main(void){

	init_Hardware();

	while(1){

	}
	return 0;
}


void init_Hardware(void){

/* Inicialización de los pines GPIO a utilizar en el display */
	/* LEDS */
	bit0.pGPIOx = GPIOA;
	bit0.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	bit0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit0.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit0.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit0);
	GPIO_WritePin(&bit0, SET);

	bit1.pGPIOx = GPIOB;
	bit1.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	bit1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit1.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit1.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit1);
	GPIO_WritePin(&bit1, SET);

	bit2.pGPIOx = GPIOB;
	bit2.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	bit2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit2.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit2);
	GPIO_WritePin(&bit2, SET);

	bit3.pGPIOx = GPIOA;
	bit3.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	bit3.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit3.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit3.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	bit3.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit3);
	GPIO_WritePin(&bit3, SET);

	bit4.pGPIOx = GPIOA;
	bit4.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	bit4.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit4.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit4.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit4.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit4);
	GPIO_WritePin(&bit4, SET);

	bit5.pGPIOx = GPIOB;
	bit5.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	bit5.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit5.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit5.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit5.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit5);
	GPIO_WritePin(&bit5, SET);

	bit6.pGPIOx = GPIOA;
	bit6.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	bit6.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit6.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit6.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit6.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit6);
	GPIO_WritePin(&bit6, SET);

	/* Timer para oscilación en transistores */
	timerDisplay.ptrTIMx = TIM3;
	timerDisplay.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timerDisplay.TIMx_Config.TIMx_period = 10; // Tiempo en milisegundos
	timerDisplay.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerDisplay.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	BasicTimer_Config(&timerDisplay);

	/* Configuración de pines para transistores */
	tranUni.pGPIOx = GPIOB;
	tranUni.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	tranUni.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	tranUni.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	tranUni.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	tranUni.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&tranUni);
	GPIO_WritePin(&tranUni, SET);

	tranDec.pGPIOx = GPIOB;
	tranDec.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	tranDec.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	tranDec.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	tranDec.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	tranDec.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&tranDec);
	GPIO_WritePin(&tranDec, RESET);

	/* Definicion de posiciones en arreglo para 7 segmentos */
	bits_7Seg[0] = bit0;
	bits_7Seg[1] = bit1;
	bits_7Seg[2] = bit2;
	bits_7Seg[3] = bit3;
	bits_7Seg[4] = bit4;
	bits_7Seg[5] = bit5;
	bits_7Seg[6] = bit6;

	// Timer para LED de estado usando el LED2
	timerLED2.ptrTIMx = TIM2;
	timerLED2.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLED2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerLED2.TIMx_Config.TIMx_period = 300; // Tiempo en milisegundos
	timerLED2.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	BasicTimer_Config(&timerLED2);

	// Controlador de LED2 asignado como led de estado
	handLED2.pGPIOx = GPIOA;
	handLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handLED2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handLED2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handLED2.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handLED2);


/* Inicialización de los pines asociados a las señales entregadas
 * por el encoder */

		// GPIO para clock de encoder
		encodCLK.pGPIOx = GPIOB;
		encodCLK.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		encodCLK.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodCLK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		// EXTI para interrupción realizada por el clock del encoder
		exti_encodCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
		exti_encodCLK.pGPIOHandler = &encodCLK;
		extInt_Config(&exti_encodCLK);

		// GPIO para señal DT del encoder que determinará la dirección del movimiento
		encodDT.pGPIOx = GPIOB;
		encodDT.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		encodDT.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&encodDT);

		// GPIO para botón del encoder que determinará cambio de función
		encodSW.pGPIOx = GPIOA;
		encodSW.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
		encodSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		// EXTI para interrupción realizada por el botón del encoder
//		exti_encodSW.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
//		exti_encodCLK.pGPIOHandler = &encodSW;
//		extInt_Config(&exti_encodSW);
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
void callback_extInt5(void){
	modo++;
//	giro_Numero(numero, encodDT);
}


/* Modo de funcion en programa*/
//void callback_extInt10(void){
//	cambio_modo(modo);
//}






