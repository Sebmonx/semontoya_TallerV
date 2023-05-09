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

#define HSI_CLOCK	0
#define HSE_CLOCK	1


typedef struct
{
	uint8_t 	input_Clock;
	uint8_t 	m_Factor;
	uint16_t	n_Factor;
	uint8_t 	p_Factor;
} PLL_Config_t;


void PLL_custom_config(PLL_Config_t *ptrPLL);
void PLL_100Mhz_Config(void);
void PLL_16Mhz_Config(void);
void PLL_Frequency_Output(GPIO_Handler_t *ptrA8);
void PLL_ON_forSystem(uint8_t frecuencia_Mhz);
void systemClock_16MHz(void);
void systemClock_100MHz(void);
void systemClock_Output(GPIO_Handler_t *ptrC9);

#endif /* PLLDRIVER_H_ */
