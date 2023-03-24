/*
 * GPIO.h
 *
 *  Created on: 11/03/2023
 *      Author: semontoya
 */

#ifndef INC_GPIOXDRIVER_H_
#define INC_GPIOXDRIVER_H_

#include <stm32f411xe.h>

typedef struct{
    uint8_t GPIO_PinNumber;         // Pin a trabajar
    uint8_t GPIO_PinMode;           // Modo de configuración: entrada, salida, análogo, funcion alt
    uint8_t GPIO_PinSpeed;          // Velocidad de respuesta
    uint8_t GPIO_PinPuPdControl;    // Selección salida pull up o pull down o libre
    uint8_t GPIO_PinOType;          // Selecciona salida PUPD o OpenDrain
    uint8_t GPIO_PinAltFunMode;     // Selecciona la función alternativa a config
} GPIO_PinConfig_t;


typedef struct
{
    GPIO_TypeDef	    *pGPIOx;        // Dirección del puerto para pin
    GPIO_PinConfig_t   	GPIO_PinConfig; // Config Pin
} GPIO_Handler_t;


// Definición de cabeceras de funciones driver principal
void GPIO_Config(GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler);
void GPIOxTooglePin1Segundo(GPIO_Handler_t *pPinHandler);




// Definicion cabecera de funciones para contador


#endif /* INC_GPIOXDRIVER_H_*/
