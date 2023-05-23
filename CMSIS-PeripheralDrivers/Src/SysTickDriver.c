/*
 * SysTickDriver.c
 *
 *  Created on: Apr 30, 2023
 *      Author: Sebastian Montoya
 */

#include <stm32f4xx.h>
#include "SysTickDriver.h"

uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;


/*  */

void config_SysTick_ms(uint8_t systemClock){
	// Reiniciamos el valor de la variable que cuenta tiempo
	ticks = 0;

	// Cargando el valor del limite de incremento en 1 ms
	switch(systemClock){

	// Caso para el reloj HSI -> 16MHz
	case 0:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;

	// Caso para el reloj HSE
	case 1:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;

	// Caso para el reloj PLL a 100MHz
	case 2:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_100MHz_1ms;
		break;

	// En caso de que llegue un valor diferente
	default:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;
	}

	// Limpiamos el valor del systick
	SysTick->VAL = 0;

	// Configuramos el reloj interno como reloj para timer
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// Desactivamos las interrupciones globales
	__disable_irq();

	// Matriculamos la interrupción en el NVIC
	NVIC_EnableIRQ(SysTick_IRQn);

	// Activamos la interrupción debido al conteo a cero del Systick
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	// Activamos el timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	// Activamos de nuevo las nterrupciones globales
	__enable_irq();

}


// Cantidad de ms que han pasado
uint8_t getTicks_ms(void){
	return ticks;
}

void delay_ms(uint32_t wait_time_ms){
	// Captura el primer valor de tiempo para comparar
	ticks_start = getTicks_ms();

	// Captura el segundo valor de tiempo para comparar
	ticks_counting = getTicks_ms();

	/* Compara si el valor counting es menor que el start + wait
	 * actual
	 * Repite esta operacion hasta que counting sea mayor */
	while(ticks_counting < (ticks_start + (uint64_t)wait_time_ms)){
		// Actualizar el valor
		ticks_counting = getTicks_ms();
	}

}


void SysTick_Handler(void){
	// Verificamos el lanzamiento de la interrupción
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		// Limpiamos la bandera
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		// Incrementamos en 1 el contador
		ticks++;
	}
}


