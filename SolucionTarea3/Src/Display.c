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
int num_Display(uint8_t numero){

	uint8_t bin = 0;

	switch(numero){

		case 0:{
			bin = 0x3F;
			break;
		}
		case 1: {
			bin = 0x06;
			break;
		}
		case 2: {
			bin = 0x5B;
			break;
		}
		case 3:{
			bin = 0x4F;
			break;
		}
		case 4:{
			bin = 0x66;
			break;
		}
		case 5:{
			bin = 0x6D;
			break;
		}
		case 6:{
			bin = 0x7D;
			break;
		}
		case 7:{
			bin = 0x07;
			break;
		}
		case 8:{
			bin = 0x7F;
			break;
		}
		case 9:{
			bin = 0x4F;
			break;
		}
		default:{
			__NOP();
		}
	}

	return bin;

}


// Funcion para enviar digito a display de décimas
void numero_Display1(uint8_t numero, GPIO_Handler_t bits[7]){

	uint8_t decima = numero/10; 	// Valor de la decima
	decima = num_Display(decima);	// Conversión a binario para display

	for(uint8_t j = 0; j < 7; j++){		// Apagado de todos los leds
		GPIO_WritePin(&bits[j], RESET);
	}

	for(uint8_t i = 0; i < 7; i++){
		/* Encendido de pin asociado al bit del número en binario */
		if(0b1 << i & decima){
			GPIO_WritePin(&bits[i], SET);
		} else {
			__NOP();
		}
	}
}


// Función para enviar digito a display de unidades
void numero_Display2(uint8_t numero, GPIO_Handler_t bits[7]){

	uint8_t unidad = numero%10;		// Valor de la unidad
	unidad = num_Display(unidad);	// Conversión a binario para display

	for(uint8_t j = 0; j < 7; j++){		// Apagado de todos los leds
		GPIO_WritePin(&bits[j], RESET);
	}

	for(uint8_t i = 0; i < 7; i++){
		/* Encendido de pin asociado al bit del número en binario */
		if(0b1 << i & unidad){
			GPIO_WritePin(&bits[i], SET);
		} else {
			__NOP();
		}
	}
}


void culebrita(uint8_t snake, GPIO_Handler_t pos_Snake[12]){
	for(uint8_t i = 0; i < 12; i++){
		if(snake == i){
			GPIO_WritePin(&pos_Snake[i], SET);
		} else {
			GPIO_WritePin(&pos_Snake[i], RESET);
		}
	}
}





