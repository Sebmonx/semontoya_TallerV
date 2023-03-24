/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya caicedo
 * @brief          : Taller 5
 ******************************************************************************
 **/
#include <stdint.h>
#define sizeOfArray 4


int main(void){

	// Ejemplo punteros
	// Definicion variables
	uint8_t  dato = 124;

	// Crear puntero con direcciòn de memoria de la variable
	uint8_t *pDato = &dato;

	// Guardar valor existente en la direccion de memoria
	uint8_t valorDato = *pDato;

	// Casteo basico
	uint16_t newDato = (uint16_t) dato;

	/* Crear un puntero a una direccion especifica, es decir, no se le da la
	libertad al sistema de elegir la direccion de memoria
	Se esta generando un puntero de 16 bits a una direccion de memoria definiendo
	el numero de 32 bits como un puntero, no como un numero */
	uint16_t *punteroDireccion = (uint16_t *) 0x20000001;


	/* Cambiar direccion de memoria del puntero */
	punteroDireccion = (uint16_t) 0x20000002;


	/* Guardar la direccion de memoria de un puntero */
	uint32_t direccionDato = (uint32_t) pDato;


	// Cambiar el valor almacenado en el puntero
	*pDato = 200;

	/* Aumentar 8 bits el puntero, cambia la posicion en la cantidad de bits
	que esta apuntando inicialmente*/
	pDato++;


	//Ejemplo arreglos
	uint8_t Arreglo[sizeOfArray] = {5, 0xAE, 'a', 254};

	uint8_t contenido = 0;

	for(uint8_t i = 0; i < sizeOfArray; i++){
		contenido = Arreglo[i];
	}

	for(uint8_t i = 0; i < sizeOfArray; i++){
			contenido = *(Arreglo + i);
	}

	// Cambiar elementos de un arreglo
	Arreglo[2] = 12;
	*(Arreglo + 2) = 12;


	// Estructura
	typedef struct {
		uint8_t elem1;
		uint16_t elem2;
		uint8_t arreglo[sizeOfArray];
	} estructura;

	estructura estructura1 ={0};

	// Acceder a elementos de estructura
	estructura1.elem1 = 1;
	estructura1.elem2 = 2;
	estructura1.arreglo = {1, 2, 3, 4};


	// Crear puntero que apunta a la estructura
	estructura *pEstructura1 = &estructura1;
	pEstructura1->elem1 = 3;



};
