/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya caicedo
 * @brief          : Taller 5
 ******************************************************************************
 **/

#include <stdint.h>


int main(void)
{
    /* Loop forever */
	while(1){
		//Definir handler para el PIN a config
			GPIO_Handler_t handlerUserLedPin = {0};

			//Deseamos trabajar con GPIOA
			handlerUserLedPin.pGPIOx = GPIOA;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinOType 			= GPIO_OTYPE_PUSHPULL;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_MEDIUM;
			handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;	//Sin función

			//Cargamos al pin específico
			GPIO_Config(&handlerUserLedPin);

			//Modifica pin para encender
			GPIO_WritePin(&handlerUserLedPin, SET);

			while(1){
				NOP();
			}
	}
}
