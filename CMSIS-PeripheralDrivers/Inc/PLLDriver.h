/*
 * PLLDriver.h
 *
 *  Created on: May 6, 2023
 *      Author: smontoya
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"


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
void PLL_100Mhz_config(void);

#endif /* PLLDRIVER_H_ */
