/*
 * ExtiDriver.c
 *
 *  Created on: 29/03/2023
 *      Author: karol
 */

#include "ExtiDriver.h"
#include "GPIOxDriver.h"

// Haciendo prueba con PC15
void extInt_Config(EXTI_Config_t *extiConfig){

	/* 1.0 Se carga la configuración, que debe ser el PINx como entrada "simple" */
	GPIO_Config(extiConfig->pGPIOHandler);

	/* 2.0 Activamos el acceso al SYSCFG */
	/*Agregue su código acá*/
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 3.0  Asignamos el canal EXTI que corresponde al PIN_y del puerto GPIO_X
	 * Debemos activar la línea PIN_Xy (Y = A, B, C... y x = 0, 1, 2, 3...)
	 * en el módulo EXTI */
	switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {
	/* Configurando para el todos los pines GPIOX_0*/
	case 0: {
		/* SYSCFG_EXTICR1 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI0_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_1*/
	case 1: {
		/* SYSCFG_EXTICR1 */
        /*Agregue su código acá*/
		// Limpiamos la posición a configurar
		SYSCFG -> EXTICR[0] &= (0xF << SYSCFG_EXTICR1_EXTI1_Pos);
		//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
		if(extiConfig->pGPIOHandler->pGPIOx == GPIOA){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PA);
		}
		else if(extiConfig->pGPIOHandler->pGPIOx == GPIOB){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PB);
		}
		else if(extiConfig->pGPIOHandler->pGPIOx == GPIOC){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PC);
		}
		else if(extiConfig->pGPIOHandler->pGPIOx == GPIOD){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PD);
		}
		else if(extiConfig->pGPIOHandler->pGPIOx == GPIOE){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PE);
		}
		else if(extiConfig->pGPIOHandler->pGPIOx == GPIOH){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PH);
		}
		else{
			__NOP();
		}
		break;
	}

	/* Configurando para el todos los pines GPIOX_2*/
	case 2: {
		/* SYSCFG_EXTICR1 */
        /*Agregue su código acá*/
		// Limpiamos la posición a configurar
		SYSCFG->EXTICR[0] &= (0xF << SYSCFG_EXTICR1_EXTI2_Pos);

		//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PA);

		}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

		}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PC);

		}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PD);

		}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PE);

		}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PH);

		}else{
			__NOP();
		}
		break;
	}
	case 3: {
			/* SYSCFG_EXTICR1 */
	        /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[0] &= (0xF << SYSCFG_EXTICR1_EXTI3_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 4: {
			/* SYSCFG_EXTICR1 */
	        /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[1] &= (0xF << SYSCFG_EXTICR2_EXTI4_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR2_EXTI4_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 5: {
			/* SYSCFG_EXTICR1 */
	        /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[1] &= (0xF << SYSCFG_EXTICR2_EXTI5_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 6: {
			/* SYSCFG_EXTICR1 */
	        /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[1] &= (0xF << SYSCFG_EXTICR2_EXTI6_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 7: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[1] &= (0xF << SYSCFG_EXTICR2_EXTI7_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 8: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[2] &= (0xF << SYSCFG_EXTICR3_EXTI8_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 9: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[2] &= (0xF << SYSCFG_EXTICR3_EXTI9_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 10: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[2] &= (0xF << SYSCFG_EXTICR3_EXTI10_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 11: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[2] &= (0xF << SYSCFG_EXTICR3_EXTI11_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 12: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[3] &= (0xF << SYSCFG_EXTICR4_EXTI12_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 13: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[3] &= (0xF << SYSCFG_EXTICR4_EXTI13_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PH);

			}else{
				__NOP();
			}
			break;
	}
	case 14: {
			/* SYSCFG_EXTICR1 */
		    /*Agregue su código acá*/
			// Limpiamos la posición a configurar
			SYSCFG->EXTICR[3] &= (0xF << SYSCFG_EXTICR4_EXTI14_Pos);

			//Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			//del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PA);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PB);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PC);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PD);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PE);

			}else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH){
				SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PH);

			}else{
				__NOP();
			}
			break;
	}

	/* Configurando para el todos los pines GPIOX_15 */
	case 15: {
		/* SYSCFG_EXTICR4 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI15_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PH);

		} else {
			__NOP();
		}
		break;
	}

	default: {
		__NOP();
		break;
	}

	}// Fin del switch-case

	/* 4.0 Seleccionamos el tipo de flanco */
	if(extiConfig->edgeType == EXTERNAL_INTERRUPT_FALLING_EDGE){
			/* Falling Trigger selection register*/
	        /*Agregue su código acá*/

			EXTI->FTSR |= (SET<< extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

		}
		else{
	        /*Agregue su código acá*/

			EXTI ->RTSR |= (SET<< extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

		}

	/* 5.0 Desactivo primero las interrupciones globales */
    /*Agregue su código acá*/
	__disable_irq();

	/* 6.0 Activamos la interrupción del canal que estamos configurando */
	// Interrupt Mask register
	/*Agregue su código acá*/

	EXTI->IMR |= (SET<< extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	/* 6.1 Matriculamos la interrupción en el NVIC para el canal correspondiente,
	 * donde el canal 0 corresponde al EXTI_0, canal 1 al EXTI_1, etc.
	 *
	 * NOTA: Observar que algunos canales EXTI comparten un mismo vector de interrupción
	 * */
	switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {
	case 0: {
		__NVIC_EnableIRQ(EXTI0_IRQn);
		break;
	}

	case 1: {
		/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI1_IRQn);
		break;
	}

	case 2: {
			/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI2_IRQn);
		break;
	}

	case 3: {
			/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI3_IRQn);
		break;
	}

	case 4: {
			/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI4_IRQn);
		break;
	}

	case 5: {
			/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 6: {
			/*Agregue su código acá*/
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 7: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
	}

	case 8: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
	}

	case 9: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
	}

	case 10: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	case 11: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	case 12: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	case 13: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	case 14: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	case 15: {
			/*Agregue su código acá*/
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
	}

	default: {
		break;
	}

	}

	/* 7.0 Volvemos a activar las interrupciones globales */
	/*Agregue su código acá*/
	__enable_irq();
}


/**/
/*
 * Agregar TODOS los demas callbacks (del 1 al 15) para un total
 * de  16 posibles interrupciones
 */

__attribute__ ((weak)) void callback_extInt0(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt1(void){
	__NOP();

}

__attribute__ ((weak)) void callback_extInt2(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt3(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt4(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt5(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt6(void){
	__NOP();

}


__attribute__ ((weak)) void callback_extInt7(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt8(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt9(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt10(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt11(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt12(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt13(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt14(void){
	__NOP();
}


__attribute__ ((weak)) void callback_extInt15(void){
	__NOP();
}


/* ISR de la interrupción canal 0*/
void EXTI0_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_0 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR0){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR0;

		// llamamos al callback
		callback_extInt0();
	}
}

/* Agregue las demas IRQs de las interrupciones EXTI independientes ...
 * Por favor recuerde que debe agregar el bloque if para verificar que
 * en efecto esa es la interrupcion que se está atendiendo.
 */

void EXT1_IRQHandler (void){
	//Evaluamos si la interrupción que se lanzó corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR1){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR1;

		// llamamos al callback
		callback_extInt1();
	}
}

void EXT2_IRQHandler (void){
	//Evaluamos si la interrupción que se lanzó corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR2){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR2;

		// llamamos al callback
		callback_extInt2();
	}
}
void EXT3_IRQHandler (void){
	//Evaluamos si la interrupción que se lanzó corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR3){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR3;

		// llamamos al callback
		callback_extInt3();
	}
}
void EXT4_IRQHandler (void){
	//Evaluamos si la interrupción que se lanzó corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR4){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR4;

		// llamamos al callback
		callback_extInt4();
	}
}
/* ISR de la interrupción canales 9_5
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR5){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR5;

		// llamamos al callback
		callback_extInt5();
	}
	else if (EXTI->PR & EXTI_PR_PR6){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR6;

		// llamamos al callback
		callback_extInt6();
	}
	else if (EXTI->PR & EXTI_PR_PR7){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR7;

		// llamamos al callback
		callback_extInt7();
	}
	else if (EXTI->PR & EXTI_PR_PR8){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR8;

		// llamamos al callback
		callback_extInt8();
	}
	else if (EXTI->PR & EXTI_PR_PR9){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR9;

		// llamamos al callback
		callback_extInt9();
	}
} //Fin pines 5 al 9



/* ISR de la interrupción canales 15_10
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR10){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR10;

		// llamamos al callback
		callback_extInt10();
	}
	else if (EXTI->PR & EXTI_PR_PR11){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR11;

		// llamamos al callback
		callback_extInt11();
	}
	else if (EXTI->PR & EXTI_PR_PR12){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR12;

		// llamamos al callback
		callback_extInt12();
	}
	else if (EXTI->PR & EXTI_PR_PR13){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR13;

		// llamamos al callback
		callback_extInt13();
	}
	else if (EXTI->PR & EXTI_PR_PR14){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR14;

		// llamamos al callback
		callback_extInt14();
	}
	else if (EXTI->PR & EXTI_PR_PR15){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR15;

		// Llamamos al callback
		callback_extInt15();
	}
} //Fin pines 10 a 15
