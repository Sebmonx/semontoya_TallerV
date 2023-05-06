/*
 * PLLDriver.c
 *
 *  Created on: May 6, 2023
 *      Author: smontoya
 */

#include "GPIOxDriver.h"
#include "PLLDriver.h"

void PLL_custom_config(PLL_Config_t *ptrPLL){

	freq = 16;
	/* Asegurar que el PLL está apagado para configuración */
	RCC->CR &= ~RCC_CR_PLLON;

	/* Selección de reloj de entrada para PLL */
	if(ptrPLL->input_Clock == HSI_CLOCK){
		RCC->PLLCFGR &= RCC_PLLCFGR_PLLSRC_HSI;
	} else {
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	}


	/* División de reloj principal para entrar a VCO */
	/* Reinicio de bits en divisor /M*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;

	/* Divisón por M */
	freq = freq/(ptrPLL->m_Factor);
	if(freq < 1 || 2 < freq){
		return 1;
	}
	RCC->PLLCFGR |= (ptrPLL->m_Factor << RCC_PLLC8FGR_PLLM_Pos);



	/* Multiplicación de salida VCO (2MHz) a 200MHz */
	/* Reinicio de bits xN */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;

	/* Multiplicación por N */
	freq = freq * (ptrPLL->n_Factor);
	if(freq < 100 || 432 < freq){
		return 1;
	}
	RCC->PLLCFGR |= (ptrPLL->n_Factor << RCC_PLLCFGR_PLLN_Msk);



	/* División de salida para sistema principal (200MHz) a 100MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;

	/* División por P */
	freq = freq/(ptrPLL->p_Factor);
	if(freq > 100){
		return 1;
	}
	RCC->PLLCFGR |= (ptrPLL->p_Factor << RRCC_PLLCFGR_PLLN_Msk);

	PLL_Frequency_Test();
}

void PLL_100Mhz_Config(void){
	freq = (16/8)*100*(1/2);
	if(freq > 100){
		return 1;
	}
	/* Asegurar que el PLL está apagado para configuración */
	RCC->CR &= ~RCC_CR_PLLON;

	/* Selección de reloj de entrada para PLL */
	RCC->PLLCFGR &= RCC_PLLCFGR_PLLSRC_HSI;

	/* División de reloj principal a 2MHz para entrar a VCO */
	/* Reinicio de bits en divisor /M*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	/* Divisón por 8 */
	RCC->PLLCFGR |= (8 << RCC_PLLC8FGR_PLLM_Pos);


	/* Multiplicación de salida VCO (2MHz) a 200MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	/* Multiplicación por 100 */
	RCC->PLLCFGR |= (100 << RCC_PLLCFGR_PLLN_Msk);


	/* División de salida para sistema principal (200MHz) a 100MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	/* División por 2 */
	RCC->PLLCFGR |= (2 << RRCC_PLLCFGR_PLLN_Msk);

}

void PLL_Frequency_Test(GPIO_Handler_t *ptrA8){

	/* Encendido GPIO A8 */
	ptrA8->pGPIOx = GPIOA;
	ptrA8->GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	ptrA8->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	ptrA8->GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(ptrA8);

	/* Encendido MCO1 */


	/* Encender señal PLL */
	RCC->CR |= RCC_CR_PLLON;
}
