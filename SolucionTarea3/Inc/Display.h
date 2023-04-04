/*
 * Display.h
 *
 *  Created on: Apr 3, 2023
 *      Author: ingfisica
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


int num_Display(uint8_t numero);
void numero_Display1(uint8_t numero, GPIO_Handler_t bits[7]);
void numero_Display2(uint8_t numero, GPIO_Handler_t bits[7]);
void culebrita(uint8_t snake, GPIO_Handler_t pos_Snake[12]);

#endif /* DISPLAY_H_ */
