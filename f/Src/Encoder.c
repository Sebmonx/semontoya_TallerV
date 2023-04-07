/*
 * Encoder.c
 *
 *  Created on: Apr 3, 2023
 *      Author: Sebastian Montoya
 */


#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"



/* Función para realizar incremento o decremento del valor que irá
 * al display restringido entre 0 y 99 leyendo el pin encodDT para
 * decidir la dirección*/
void giro_Numero(uint8_t numero, GPIO_Handler_t senal_Dir){
	if(GPIO_ReadPin(&senal_Dir)){		// Dirección
		if(numero == 99){			// Limitante 99
			__NOP();
		} else {
			numero++;				// Incremento
		}
	} else{
		if(numero == 0){			// Limitente 0
			__NOP();
		} else {
			numero--;				// Decremento
		}
	}
}


/* Función para cambiar el modo de ejecución en el display
 * utiliza una variable que cambia cada que se presione el
 * botón del encoder */
void cambio_modo(uint8_t modo){
	modo ^= 0b1;
}

