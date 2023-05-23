/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoyas
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>


int main(void)
{
	/* Activador coprocesador matemático - FPU */
	//SCB->CPACR |= (0xF << 20);

    /* Loop forever */
	while(1){

	}

	return 0;
}
