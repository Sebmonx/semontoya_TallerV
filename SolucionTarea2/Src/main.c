/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya caicedo
 * @brief          : Taller 5
 ******************************************************************************
 **/

#include <stdint.h>
#include "GPIOxDriver.h"


int main(void)
{
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

	uint32_t pin = 0;

	pin = GPIO_ReadPin(&handlerUserLedPin);
	pin = GPIO_ReadPin(&handlerUserLedPin);

	GPIO_Handler_t handlerButton = {0};
	handlerButton.pGPIOx = GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerButton.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerButton);


	GPIO_Handler_t handlerPC9= {0};

	handlerPC9.pGPIOx = GPIOC;
	handlerPC9.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerPC9.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPC9.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPC9.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPC9.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPC9.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPC9);
	GPIO_WritePin(&handlerPC9, SET);


	GPIO_Handler_t handlerPC6= {0};

	handlerPC6.pGPIOx = GPIOC;
	handlerPC6.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	handlerPC6.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPC6.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPC6.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPC6.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPC6.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPC6);
	GPIO_WritePin(&handlerPC6, SET);



	GPIO_Handler_t handlerPB8= {0};

	handlerPB8.pGPIOx = GPIOB;
	handlerPB8.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handlerPB8.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPB8.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPB8.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPB8.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPB8.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPB8);
	GPIO_WritePin(&handlerPB8, SET);


	GPIO_Handler_t handlerPA6= {0};

	handlerPA6.pGPIOx = GPIOA;
	handlerPA6.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	handlerPA6.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPA6.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPA6.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPA6.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPA6.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPA6);
	GPIO_WritePin(&handlerPA6, SET);



	GPIO_Handler_t handlerPC7= {0};

	handlerPC7.pGPIOx = GPIOC;
	handlerPC7.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	handlerPC7.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPC7.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPC7.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPC7.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPC7.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPC7);
	GPIO_WritePin(&handlerPC7, SET);


	GPIO_Handler_t handlerPC8= {0};

	handlerPC8.pGPIOx = GPIOC;
	handlerPC8.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handlerPC8.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPC8.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPC8.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPC8.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPC8.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPC8);
	GPIO_WritePin(&handlerPC8, SET);


	GPIO_Handler_t handlerPA7= {0};

	handlerPA7.pGPIOx = GPIOA;
	handlerPA7.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	handlerPA7.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerPA7.GPIO_PinConfig.GPIO_PinOType 		= GPIO_OTYPE_PUSHPULL;
	handlerPA7.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPA7.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_MEDIUM;
	handlerPA7.GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
	GPIO_Config(&handlerPA7);
	GPIO_WritePin(&handlerPA7, SET);

	/*	Arreglo de Pines en orden de bits
		Puede ser un arreglo de 7 elementos, a menos que sea un string no necesita
	 	el espacio extra para el caracter NULL*/
	GPIO_Handler_t bits[8] = {handlerPA7, handlerPC8, handlerPC7, handlerPA6,
								handlerPB8, handlerPC6, handlerPC9};

	// Numero maximo a contar en binario
	uint8_t n = 60;

	while(1){

		// Ciclo de incremento
		for(uint8_t i = 1; i <= n;i++){

			// Led2 acompañando el ciclo
			GPIOxTooglePin(&handlerUserLedPin);

			// Delay
			for(uint32_t o = 0; o < 1000000; o++){
				NOP();
			}

			// Limpieza de pines
			for(uint8_t j = 0; j < 7; j++){
				GPIO_WritePin(&bits[j], RESET);
			}

			// Escritura en bits de numero
			for(uint8_t j = 0; j < 7; j++){

				// Mascara auxiliar
				uint8_t mask = 0b1;

				// Condicional para escribir solo en bits necesarios
				if(mask << j & i){
					GPIO_WritePin(&bits[j], SET);
				} else {
					NOP();
				}

			}

			// Condicional de decrecimiento al presionar boton
			if(!GPIO_ReadPin(&handlerButton)){
				i -= 2;

				// Condicional para regresar a valor n y continuar decreciendo
				if(i == -1){
					i = n;
				} else {
					NOP();
				}
			} else {
				NOP();
			}
		}

	}

}

