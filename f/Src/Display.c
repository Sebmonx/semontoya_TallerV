/*
 * Display.c
 *
 *  Created on: Apr 3, 2023
 *      Author: Sebastian Montoya
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/* Función que convierte número entre 0 y 9 al
 * binario equivalente para el display 7 segmentos
 * Este caso es de cátodo común */
uint8_t numero_7Segmentos(uint8_t numero){

	uint8_t bin = 0;

	switch(numero){

		case 0:{
			bin = 0b0111111;
			break;
		}
		case 1: {
			bin = 0b0000110;
			break;
		}
		case 2: {
			bin = 0b1011011;
			break;
		}
		case 3:{
			bin = 0b1001111;
			break;
		}
		case 4:{
			bin = 0b1100110;
			break;
		}
		case 5:{
			bin = 0b1101101;
			break;
		}
		case 6:{
			bin = 0b1111101;
			break;
		}
		case 7:{
			bin = 0b0000111;
			break;
		}
		case 8:{
			bin = 0b1111111;
			break;
		}
		case 9:{
			bin = 0b1101111;
			break;
		}
		default:{
			__NOP();
		}
	}

	return bin;

}


// Funcion para enviar digito a display de décimas
void numero_Display(uint8_t numero, GPIO_Handler_t bits[7]){

	uint8_t auxVar = numero_7Segmentos(numero); // Conversión a binario para display

	for(uint8_t j = 0; j < 7; j++){		// Apagado de todos los leds
		GPIO_WritePin(&bits[j], RESET);
	}

	for(uint8_t i = 0; i < 7; i++){
		/* Encendido de pin asociado al bit del número en binario */
		if(0b1 << i & auxVar){
			GPIO_WritePin(&bits[i], SET);
		} else {
			__NOP();
		}
	}
}


// Función para enviar digito a display de unidades
//void unidad_Display(uint8_t numero, GPIO_Handler_t bits[7],GPIO_Handler_t transistor){
//
//	uint8_t unidad = numero%10;		// Valor de la unidad
//	uint8_t auxVar = numero_7Segmentos(unidad); // Conversión a binario para display
//
//	for(uint8_t j = 0; j < 7; j++){		// Apagado de todos los leds
//		GPIO_WritePin(&bits[j], RESET);
//	}
//
//	for(uint8_t i = 0; i < 7; i++){
//		/* Encendido de pin asociado al bit del número en binario */
//		if(0b1 << i & auxVar){
//			GPIO_WritePin(&bits[i], SET);
//		} else {
//			__NOP();
//		}
//	}
//	GPIO_WritePin(transistor, RESET);
//
//}



void culebrita(uint8_t snake, GPIO_Handler_t pos_Snake[12]){
	for(uint8_t i = 0; i < 12; i++){
		if(snake == i){
			GPIO_WritePin(&pos_Snake[i], SET);
		} else {
			GPIO_WritePin(&pos_Snake[i], RESET);
		}
	}
}





