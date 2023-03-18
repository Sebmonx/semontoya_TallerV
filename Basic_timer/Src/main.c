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


// Definicion elemento
GPIO_Handler_t handlerLed2 = {0}; //Pin 5

int main(void)
{
	// Configurar el pin utilizando la estructura para handlers
	handlerLed2.pGPIOx 									= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	// Cargar configuracion a Pin A5
	GPIO_Config(&handlerLed2);




    /* Loop forever */
	while(1){

	}
}
