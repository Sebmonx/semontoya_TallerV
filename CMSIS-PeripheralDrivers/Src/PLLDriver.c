/*
 * PLLDriver.c
 *
 *  Created on: May 6, 2023
 *      Author: smontoya
 */

#include "GPIOxDriver.h"
#include "PLLDriver.h"
#include <string.h>


void PLL_config(PLL_Config_t *ptrPLL){
	if(ptrPLL->final_Frequency == 100){
		PLL_100Mhz_Config();
	}
	else if(ptrPLL->final_Frequency == 16){
		PLL_16Mhz_Config();
	}
	else{
		uint8_t actual_Frequency = 16;

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
		actual_Frequency = actual_Frequency/(ptrPLL->m_Factor);

		// No configurar si supera los límites
		if(actual_Frequency < 1 || 2 < actual_Frequency){
			while(1){
				// Supera los límites
				__NOP();
			}
		}
		// Configurar factor M
		RCC->PLLCFGR |= (ptrPLL->m_Factor << RCC_PLLCFGR_PLLM_Pos);


		/* Multiplicación de salida VCO (2MHz) a 200MHz */
		/* Reinicio de bits xN */
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;

		/* Multiplicación por N */
		actual_Frequency = actual_Frequency * (ptrPLL->n_Factor);

		// No configurar si supera los límites
		if(actual_Frequency < 100 || 432 < actual_Frequency){
			while(1){
				// Supera los límites
				__NOP();
			}
		}
		// Configurar factor N
		RCC->PLLCFGR |= (ptrPLL->n_Factor << RCC_PLLCFGR_PLLN_Pos);


		/* División de salida para sistema principal (200MHz) a 100MHz */
		/* Reinicio de bits */
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;

		/* División por P */
		actual_Frequency = actual_Frequency/(ptrPLL->p_Factor);
		// No configurar si supera los límites
		if(actual_Frequency > 100){
			while(1){
				// Supera los límites
				__NOP();
			}
		}
		// Configurar factor P
		RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLN_Pos);

		/* Guardar frecuencia final configurada */
		ptrPLL->final_Frequency = actual_Frequency;
	}
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

// Sistema para encender PLL a configuración seleccionada
void PLL_ON_forSystem(PLL_Config_t *ptrPLL){

	/* Configuración de memoria flash y buses para nueva frecuencia */
	FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk; 	// Reset flash
	RCC->CFGR &= ~RCC_CFGR_PPRE1;			// Reset APB1
	RCC->CFGR &= ~RCC_CFGR_PPRE2;			// Reset APB2
	RCC->CFGR &= ~RCC_CFGR_HPRE;			// Reset AHB

	if(ptrPLL->final_Frequency <= 30){
		FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
	}
	else if (ptrPLL->final_Frequency <= 64){
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;

		if(ptrPLL->final_Frequency > 50){
			/* División mínima para no superar limites del bus */
			RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
		}
	}
	else if (ptrPLL->final_Frequency <= 90){
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
		/* División mínima para no superar limites del bus */
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	}
	else {
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
		/* División mínima para no superar limites del bus */
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	}

	// Endender señal de PLL
	RCC->CR |= RCC_CR_PLLON;


	/* Confirmación de PLL listo */
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	/* Configuración multiplexor que envia reloj a todo el hardware */
	/* Enviar señal PLL */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
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

	/* División mínima para no superar limites del bus APB1 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

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

void systemClock_GetConfig(system_Clock_data *ptrClockData){
	/* Fuente de reloj actual */
	if(RCC->CFGR & RCC_CFGR_SWS_HSI){
		strcpy(ptrClockData->clock_Source, "HSI");
	}
	else if(RCC->CFGR & RCC_CFGR_SWS_HSE){
		strcpy(ptrClockData->clock_Source, "HSE");
	}
	else if(RCC->CFGR & RCC_CFGR_SWS_PLL){
		strcpy(ptrClockData->clock_Source, "PLL");
	}

	/* Frecuencia actual en reloj */
	uint8_t factor_aux = 0;
	// Definir y dividir Factor M
	factor_aux = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM)>> RCC_PLLCFGR_PLLM_Pos; // Definir Factor M
	ptrClockData->clock_Frequency = 16/factor_aux;
	// Definir y multiplicar Factor N
	factor_aux = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN)>> RCC_PLLCFGR_PLLN_Pos;
	ptrClockData->clock_Frequency = (ptrClockData->clock_Frequency)*factor_aux;
	// Definir y dividir Factor P
	factor_aux = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP)>> RCC_PLLCFGR_PLLP_Pos;
	ptrClockData->clock_Frequency = (ptrClockData->clock_Frequency)/factor_aux;

	/* Frecuencia APB1 */
	switch(RCC->CFGR & RCC_CFGR_PPRE1){
		case RCC_CFGR_PPRE1_DIV1:{
			ptrClockData->APB1_Frequency = ptrClockData->clock_Frequency;
			break;
		}
		case RCC_CFGR_PPRE1_DIV2:{
			ptrClockData->APB1_Frequency = ptrClockData->clock_Frequency/2;
			break;
		}
		case RCC_CFGR_PPRE1_DIV4:{
			ptrClockData->APB1_Frequency = ptrClockData->clock_Frequency/4;
			break;
		}
		case RCC_CFGR_PPRE1_DIV8:{
			ptrClockData->APB1_Frequency = ptrClockData->clock_Frequency/8;
			break;
		}
		case RCC_CFGR_PPRE1_DIV16:{
			ptrClockData->APB1_Frequency = ptrClockData->clock_Frequency/16;
			break;
		}
		default:{
			__NOP();
			break;
		}
	}

	/* Frecuencia APB2 */
	switch(RCC->CFGR & RCC_CFGR_PPRE2){
		case RCC_CFGR_PPRE2_DIV1:{
			ptrClockData->APB2_Frequency = ptrClockData->clock_Frequency;
			break;
		}
		case RCC_CFGR_PPRE2_DIV2:{
			ptrClockData->APB2_Frequency = ptrClockData->clock_Frequency/2;
			break;
		}
		case RCC_CFGR_PPRE2_DIV4:{
			ptrClockData->APB2_Frequency = ptrClockData->clock_Frequency/4;
			break;
		}
		case RCC_CFGR_PPRE2_DIV8:{
			ptrClockData->APB2_Frequency = ptrClockData->clock_Frequency/8;
			break;
		}
		case RCC_CFGR_PPRE2_DIV16:{
			ptrClockData->APB2_Frequency = ptrClockData->clock_Frequency/16;
			break;
		}
		default:{
			__NOP();
			break;
		}
	}

	/* Frecuencia AHB */
	switch(RCC->CFGR & RCC_CFGR_HPRE){
		case RCC_CFGR_HPRE_DIV1:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency;
			break;
		}
		case RCC_CFGR_HPRE_DIV2:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/2;
			break;
		}
		case RCC_CFGR_HPRE_DIV4:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/4;
			break;
		}
		case RCC_CFGR_HPRE_DIV8:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/8;
			break;
		}
		case RCC_CFGR_HPRE_DIV16:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/16;
			break;
		}
		case RCC_CFGR_HPRE_DIV64:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/64;
			break;
		}
		case RCC_CFGR_HPRE_DIV128:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/128;
			break;
		}
		case RCC_CFGR_HPRE_DIV256:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/256;
			break;
		}
		case RCC_CFGR_HPRE_DIV512:{
			ptrClockData->AHB_Frequency = ptrClockData->clock_Frequency/512;
			break;
		}
		default:{
			__NOP();
			break;
		}
	}
	/* Latencia FLASH */
	/* Pendiente */
}
