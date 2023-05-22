/*
 * PLLDriver.h
 *
 *  Created on: May 6, 2023
 *      Author: smontoya
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include <string.h>

#define HSI_CLOCK	0
#define HSE_CLOCK	1


typedef struct
{
	uint8_t 	input_Clock; 		// Reloj a utilizar para la frecuencia del sistema
	uint8_t 	m_Factor;			// Factor de división para VCO
	uint16_t	n_Factor;			// Factor de multiplicación VCO
	uint8_t 	p_Factor;			// Factor de división para salida PLL
	uint8_t		final_Frequency;	// Variable para uso futuro, no necesita modificación
} PLL_Config_t;

typedef struct
{
	char		clock_Source[4];
	uint8_t		clock_Frequency;
	uint8_t		APB1_Frequency;
	uint8_t		APB2_Frequency;
	uint8_t		AHB_Frequency;
} system_Clock_data;		// Estructura para guardar datos informativos sobre el reloj


void PLL_config(PLL_Config_t *ptrPLL);
void PLL_Frequency_Output(GPIO_Handler_t *ptrA8);
void PLL_ON_forSystem(PLL_Config_t *ptrPLL);
void systemClock_16MHz(PLL_Config_t *ptrPLL);
void systemClock_80MHz(PLL_Config_t *ptrPLL);
void systemClock_100MHz(PLL_Config_t *ptrPLL);
void systemClock_Output(GPIO_Handler_t *ptrC9);
void systemClock_GetConfig(system_Clock_data *ptrClockData);

#endif /* PLLDRIVER_H_ */
