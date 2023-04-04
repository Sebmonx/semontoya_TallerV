/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya caicedo
 * @brief          : Taller 5
 ******************************************************************************
 **/
#include <stm32f4xx.h>
#include <stdint.h>
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
	GPIO_Handler_t handPB4 = {0};
	GPIO_Handler_t handPB5 = {0};
	GPIO_Handler_t handPB3 = {0};
	GPIO_Handler_t handPA10 = {0};
	GPIO_Handler_t handPA2 = {0};

	/* Arreglo para encendido de LEDs 7 segmentos */
	GPIO_Handler_t bits_7Seg[7] = {0};

	/* Arreglo para encendido de culebrita */
	GPIO_Handler_t pos_snake[12] = {0};

	/* Handlers GPIO transistores */
	GPIO_Handler_t tranDec = {0};
	GPIO_Handler_t tranUni = {0};

/* Variables Encoder */
	EXTI_Config_t encodCLK = {0};		// Pin para clock de encoder
	GPIO_Handler_t encodDT = {0};		// Pin para señal alternativa encoder
	EXTI_Config_t encodSW = {0}; 		// Pin para botón de cambio de función

/* Variables main */
	BasicTimer_Handler_t timerLED2 = {0};
	GPIO_Handler_t handLED2 = {0};
	uint8_t numero = 0;
	uint8_t snake = 0;
	uint8_t modo = 0;

/* Funciones a utilizar */
	void init_Hardware(void);
	void init_Display(void);
	void init_Encoder(void);


int main(void)
{
	init_Hardware();
	init_Display();
	init_Encoder();
	num_Display(numero);

	while(1){
		if(!modo){
			numero_Display1(numero, bits_7Seg);
			numero_Display2(numero, bits_7Seg);
		} else {
			culebrita(numero,pos_snake);
		}
	}
}

/* Inicializacion de hardware micro */
void init_Hardware(void){

	// Timer para LED de estado usando el LED2
	timerLED2.ptrTIMx = TIM2;
	timerLED2.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLED2.TIMx_Config.TIMx_period = 250; // Tiempo en milisegundos
	timerLED2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
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

	handPB10.pGPIOx = GPIOB;
	handPB10.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handPB10.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handPB10.GPIO_PinConfig.GPIO_PinOType 			= GPIO_OTYPE_PUSHPULL;
	handPB10.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB10.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB10);

	handPB4.pGPIOx = GPIOB;
	handPB4.GPIO_PinConfig.GPIO_PinNumber 		= PIN_4;
	handPB4.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPB4.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPB4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB4.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB4);

	handPB5.pGPIOx = GPIOB;
	handPB5.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handPB5.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPB5.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPB5.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB5.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB5);

	handPB3.pGPIOx = GPIOB;
	handPB3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handPB3.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPB3.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPB3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPB3.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPB3);

	handPA10.pGPIOx = GPIOA;
	handPA10.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handPA10.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handPA10.GPIO_PinConfig.GPIO_PinOType 			= GPIO_OTYPE_PUSHPULL;
	handPA10.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPA10.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPA10);

	handPA2.pGPIOx = GPIOA;
	handPA2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handPA2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handPA2.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handPA2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handPA2.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handPA2);

	/* Timer */
	timerDisplay.ptrTIMx = TIM3;
	timerDisplay.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	timerDisplay.TIMx_Config.TIMx_period			= 30; // Tiempo en milisegundos
	timerDisplay.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	timerDisplay.TIMx_Config.TIMx_interruptEnable 	= 1; // Activar interrupción
	BasicTimer_Config(&timerDisplay);

	/*todo Organizar los pines en el arreglo con orden adecuado según display*/

	/* Transistores */
	tranUni.pGPIOx = GPIOB;
	tranUni.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	tranUni.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	tranUni.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	tranUni.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	tranUni.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&tranUni);
	GPIO_WritePin(&tranUni, SET);

	tranDec.pGPIOx = GPIOC;
	tranDec.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	tranDec.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	tranDec.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	tranDec.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	tranDec.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&tranDec);
	GPIO_WritePin(&tranDec, RESET);

}


/* Inicialización de los pines asociados a las señales entregadas
 * por el encoder */
void init_Encoder(void){

	// Señal del encoder que dirigirá la interrupción EXTI11
	encodCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	encodCLK.pGPIOHandler->pGPIOx = GPIOA;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodCLK.pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	extInt_Config(&encodCLK);

	// Señal del encoder que determinará la dirección del movimiento
	encodDT.pGPIOx = GPIOA;
	encodDT.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	encodDT.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&encodDT);

	// Señal de encoder que lidera la interrupción EXTI12 de cambio de función
	encodSW.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	encodSW.pGPIOHandler->pGPIOx = GPIOB;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	encodSW.pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	extInt_Config(&encodSW);

}


/* Centelleo de LED de estado */
void BasicTimer2_CallBack(void){
	GPIOxTooglePin(&handLED2);
}

/* Alternar encendido de transistores */
void BasicTimer3_CallBack(void){
	GPIOxTooglePin(&tranUni);
	GPIOxTooglePin(&tranDec);
}

/* Incremento o decremento por encoder */
void callback_extInt11(void){
	giro_Numero(numero, encodDT);
}


/* Modo de funcion en programa*/
void callback_extInt12(void){
	cambio_modo(modo);
}
