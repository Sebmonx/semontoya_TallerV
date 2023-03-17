/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya caicedo
 * @brief          : Taller 5
 ******************************************************************************
 **/

#include <stdint.h>
#include "GPIOxDriver.h"

#define SUMA '+'
#define RESTA '-'
#define MULTIPLICACION '*'
#define DIVISION '/'
#define FIBONACCI '1'
#define FACTORIAL '2'


// funcion switch case
uint16_t resultadoOperaciòn(uint8_t tipoDeOperaciòn, uint8_t numA, uint8_t numB);
uint32_t funcionesF(uint8_t tipoDeOperacion, uint8_t numero);


int main(void)
{
	uint16_t operacionFinal = 0;
	operacionFinal = resultadoOperaciòn(SUMA, 7, 5);
	operacionFinal++;
	operacionFinal = 0;
	operacionFinal = funcionesF(FIBONACCI, 4);
	operacionFinal = funcionesF(FACTORIAL, 4);
	operacionFinal = funcionesF(FIBONACCI, 5);

	operacionFinal = 0;

}


uint16_t resultadoOperaciòn(uint8_t tipoDeOperacion, uint8_t numA, uint8_t numB){

		uint16_t resultado = 0;

		switch(tipoDeOperacion){

			case SUMA:{
				resultado = numA + numB;
				break;
			}
			case RESTA:
			{
				resultado = numA - numB;
				break;
			}
			case MULTIPLICACION:
			{
				resultado = numA * numB;
				break;
			}
			case DIVISION:
			{
				resultado = numA / numB;
				break;
			}
			default:
			{
				resultado = 0;
				break;
			}
		}

		return resultado;
	}

uint32_t funcionesF(uint8_t tipoDeOperacion, uint8_t numero){

	uint32_t resultado = 0;


	switch(tipoDeOperacion){

		case FIBONACCI:{
			// Contador
			uint8_t i = 0;

			while(i <= numero){
				resultado += i;
				i++;
			}
			break;
		}

		case FACTORIAL:{
			// Contador
			uint8_t i = numero;
			resultado = 1;

			while(i != 0){
				resultado *= i;
				i--;
			}
			break;
		}

		default:{}
	}

	return resultado;
}








