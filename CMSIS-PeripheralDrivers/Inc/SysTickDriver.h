/*
 * SysTickDriver.h
 *
 *  Created on: Apr 30, 2023
 *      Author: Sebastian Montoya
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include <stm32f4xx.h>

#define SYSTICK_LOAD_VALUE_16MHz_1ms	16000	// Numero de ciclos en 1 ms
#define SYSTICK_LOAD_VALUE_80MHz_1ms	80000	// Ciclos en 1 ms
#define SYSTICK_LOAD_VALUE_100MHz_1ms	100000	// Numero de ciclos en 1 ms


void config_SysTick_ms(uint8_t systemClock);
uint8_t getTicks_ms(void);
void delay_ms(uint32_t wait_time_ms);


#endif /* SYSTICKDRIVER_H_ */
