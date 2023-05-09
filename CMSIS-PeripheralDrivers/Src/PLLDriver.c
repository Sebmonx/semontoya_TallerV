/*
 * PLLDriver.c
 *
 *  Created on: May 6, 2023
 *      Author: smontoya
 */

#include "GPIOxDriver.h"
#include "PLLDriver.h"

void PLL_custom_config(PLL_Config_t *ptrPLL){

	uint8_t frequency = 16;
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
	frequency = frequency/(ptrPLL->m_Factor);
	// No configurar si supera los límites
	if(frequency < 1 || 2 < frequency){
		PLL_16Mhz_Config();
		while(1){
			__NOP();
		}
	}
	// Configurar factor M
	RCC->PLLCFGR |= (ptrPLL->m_Factor << RCC_PLLCFGR_PLLM_Pos);


	/* Multiplicación de salida VCO (2MHz) a 200MHz */
	/* Reinicio de bits xN */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;

	/* Multiplicación por N */
	frequency = frequency * (ptrPLL->n_Factor);

	// No configurar si supera los límites
	if(frequency < 100 || 432 < frequency){
		PLL_16Mhz_Config();
		while(1){
			__NOP();
		}
	}
	// Configurar factor N
	RCC->PLLCFGR |= (ptrPLL->n_Factor << RCC_PLLCFGR_PLLN_Pos);



	/* División de salida para sistema principal (200MHz) a 100MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;

	/* División por P */
	frequency = frequency/(ptrPLL->p_Factor);
	// No configurar si supera los límites
	if(frequency > 100){
		PLL_16Mhz_Config();
		while(1){
			__NOP();
		}
	}
	// Configurar factor P
	RCC->PLLCFGR |= (ptrPLL->p_Factor << RCC_PLLCFGR_PLLN_Pos);

}

void PLL_100Mhz_Config(void){
	uint8_t frequency = (16/8)*100*(1/2);

	// Si la frecuencia supera 100Mhz no realizar configuración
	while(frequency > 100){
		__NOP();
	}
	/* Asegurar que el PLL está apagado para configuración */
	RCC->CR &= ~RCC_CR_PLLON;

	/* Selección de reloj de entrada para PLL */
	RCC->PLLCFGR &= RCC_PLLCFGR_PLLSRC_HSI;

	/* División de reloj principal a 2MHz para entrar a VCO */
	/* Reinicio de bits en divisor /M*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	/* Divisón por 8 */
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);


	/* Multiplicación de salida VCO (2MHz) a 200MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	/* Multiplicación por 100 */
	RCC->PLLCFGR |= (100 << RCC_PLLCFGR_PLLN_Pos);


	/* División de salida para sistema principal (200MHz) a 100MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	/* División por 2 */
	RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLN_Pos);

}

void PLL_Frequency_Output(GPIO_Handler_t *ptrA8){

	/* Encendido GPIO A8 */
	ptrA8->pGPIOx = GPIOA;
	ptrA8->GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	ptrA8->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	ptrA8->GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(ptrA8);

	/* Configuración prescaler de MCO1 PLL/5 */
	RCC->CFGR &= ~RCC_CFGR_MCO1PRE_Msk;
	RCC->CFGR |= (0b111 << RCC_CFGR_MCO1PRE_Pos);

	/* Encendido señal para lectura MCO1 */
	RCC->CFGR |= RCC_CFGR_MCO1;

	// Endender señal de PLL
	RCC->CR |= RCC_CR_PLLON;

	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

}

void PLL_ON_forSystem(uint8_t frecuencia_Mhz){
	/* Configuración de memoria flash para nueva frecuencia */
	if(frecuencia_Mhz <= 30){
		FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
	}
	else if (frecuencia_Mhz <= 64){
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	}
	else if (frecuencia_Mhz <= 90){
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	}
	else {
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	}

	/* Confirmación de PLL listo */
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	/* Configuración multiplexor que envia reloj a todo el hardware */
	/* Enviar señal PLL */
	RCC->CFGR |= RCC_CFGR_SWS_PLL;
}

void systemClock_16MHz(void){

	/* Apaga y configura PLL */
	PLL_16Mhz_Config();

	/* Chequear HSI listo */
	while(!(RCC->CR & RCC_CR_HSIRDY)){
		__NOP();
	}

	/* Configuración de memoria flash para nueva frecuencia */
	FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;

	/* Configuración multiplexor que envia reloj a todo el hardware */
	/* Enviar señal desde HSI */
	RCC->CFGR &= RCC_CFGR_SWS_HSI;
}

void systemClock_100MHz(void){

	PLL_100Mhz_Config();

	// Endender señal de PLL
	RCC->CR |= RCC_CR_PLLON;

	/* Confirmación de PLL listo */
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	/* Configuración de memoria flash para nueva frecuencia */
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

	/* Configuración multiplexor que envia reloj a todo el hardware */
	/* Enviar señal PLL */
	RCC->CFGR |= RCC_CFGR_SWS_PLL;
}

void PLL_16Mhz_Config(void){

	/* Asegurar que el PLL está apagado para configuración */
	RCC->CR &= ~RCC_CR_PLLON;

	/* Selección de reloj de entrada para PLL */
	RCC->PLLCFGR &= RCC_PLLCFGR_PLLSRC_HSI;

	/* División de reloj principal a 2MHz para entrar a VCO */
	/* Reinicio de bits en divisor /M*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	/* Divisón por 8 */
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);


	/* Multiplicación de salida VCO (2MHz) a 160MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	/* Multiplicación por 100 */
	RCC->PLLCFGR |= (64 << RCC_PLLCFGR_PLLN_Pos);


	/* División de salida para sistema principal (200MHz) a 100MHz */
	/* Reinicio de bits */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	/* División por 2 */
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLN_Pos);

}

void systemClock_Output(GPIO_Handler_t *ptrC9){
	/* Encendido GPIO A8 */
	ptrC9->pGPIOx = GPIOC;
	ptrC9->GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	ptrC9->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	ptrC9->GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(ptrC9);

	/* Configuración prescaler de MCO2 SysCLK/5 */
	RCC->CFGR &= ~RCC_CFGR_MCO2PRE_Msk;
	RCC->CFGR |= (0b111 << RCC_CFGR_MCO2PRE_Pos);

	/* Encendido señal para lectura MCO2 */
	RCC->CFGR |= RCC_CFGR_MCO2;

}


