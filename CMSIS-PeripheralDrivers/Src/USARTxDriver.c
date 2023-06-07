/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: Sebastian Montoya
 */

#include <stm32f4xx.h>
#include <string.h>
#include <stdlib.h>
#include "USARTxDriver.h"
#include "PLLDriver.h"


uint8_t auxiliar_data_RX = 0;
uint8_t auxiliar_data_TX = 0;
uint8_t used_USART = 0;

uint8_t newData = 1;
uint8_t dataType = 0;
uint8_t contador = 0;
char dataToSend = 0;
char *buffer;
/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		// Escriba acá su código
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}
	
    /* 1.2 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		if(ptrUsartHandler->USART_Config.MCU_frequency > 50){
			ptrUsartHandler->USART_Config.MCU_frequency = ptrUsartHandler->USART_Config.MCU_frequency/2;
		}
		else {
		ptrUsartHandler->USART_Config.MCU_frequency = ptrUsartHandler->USART_Config.MCU_frequency;
		}
	}
    
    /* 1.3 Configuramos el USART6 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}
	else{
		__NOP();
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;
		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PS;
			
		} else {
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;
		}
	}
	else {
		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE;
	}

	// 2.3 Configuramos el tamaño del dato
    if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){
    	ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M;
    }
    else {
    	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;
    }

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
		case USART_STOPBIT_1: {
			// Debemoscargar el valor 0b00 en los dos bits de STOP
			ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
			break;
		}
		case USART_STOPBIT_0_5: {
			// Debemoscargar el valor 0b01 en los dos bits de STOP
			ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
			ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
			break;
		}
		case USART_STOPBIT_2: {
			// Debemoscargar el valor 0b10 en los dos bits de STOP
			ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
			ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
			break;
		}
		case USART_STOPBIT_1_5: {
			// Debemoscargar el valor 0b11 en los dos bits de STOP
			ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;
			break;
		}
		default: {
			// En el caso por defecto seleccionamos 1 bit de parada
			ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
			break;
		}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	switch(ptrUsartHandler->USART_Config.MCU_frequency){
		case 16:{
			// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
				// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
				// Valor a cargar 0x0683
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
				// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
				// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
				// Valor a cargar 0x0341
				// Configurand el Baudrate generator para una velicudad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// Valor a cargar es 8.805
				// Mantiza = 8 = 0x8, fraccion = 16 * 0.68056 = 11 = 0xB
				// Valor a cargar 0x08B
				// Configurando el baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x8B;
			}
			break;
		}

		case 40:{
			// Frec = 40MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200){
				ptrUsartHandler->ptrUSARTx->BRR = 0x0823;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				ptrUsartHandler->ptrUSARTx->BRR = 0x15B;
			}
			break;
		}

//		case 50:{
//			// Frec = 40MHz, overr = 0;
//			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
//				ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
//			}
//			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200){
//				ptrUsartHandler->ptrUSARTx->BRR = 0x0823;
//			}
//			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
//				ptrUsartHandler->ptrUSARTx->BRR = 0x15B;
//			}
//			break;
//		}

		case 80:{
			// Frec = 80MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 520,833
				// Mantiza = 527 = 0x20F, fraction = 16 * 0,34375 = 6 = 0x6
				// Valor 80 entero 0x208D
				ptrUsartHandler->ptrUSARTx->BRR = 0x208D;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200){
				// El valor a cargar es 260,41666
				// Mantiza = 263 = 0x107, fraction = 16 * 0,67187 = 11 = 0xB
				// Valor 80 entero 0x1047
				ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// El valor a cargar es 43,40277
				// Mantiza = 43 = 0x2B, fraction = 16 * 0,93453 = 15 = 0xF
				// Valor 80 entero 0x2B7
				ptrUsartHandler->ptrUSARTx->BRR = 0x02B7;
			}
			break;
		}

		case 100:{
			// Frec = 100MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 651,0416
				// Mantiza = 651 = 0x28B, fraction = 16 * 0.0416 = 1 = 0x1
				// Valor a cargar 0x28B1
				ptrUsartHandler->ptrUSARTx->BRR = 0x28B1;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200){
				// El valor a cargar es 325,52083
				// Mantiza = 325 = 0x145, fraction = 16 * 0,52083 = 8 = 0x8
				// Valor a cargar 0x1458
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
			}
			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// El valor a cargar es 54,253472
				// Mantiza = 54 = 0x36, fraction = 16 * 0,253472 = 4 = 0x4
				// Valor a cargar 0x0364
				ptrUsartHandler->ptrUSARTx->BRR = 0x0364;
			}
			break;
		}
		default:{
			while(1){
				/* Wrong frequency value*/
				__NOP();
			}
		}
	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		break;
	}
	}



	// Desactivar interrupciones globales
	__disable_irq();

	// Chequeo de activación de interrupción en recepción
	if(ptrUsartHandler->USART_Config.USART_enableIntRX == USART_INTERRUPT_TX_ENABLE){

		ptrUsartHandler->ptrUSARTx->CR1	&= ~USART_CR1_RXNEIE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;
	} else {
		ptrUsartHandler->ptrUSARTx->CR1	&= ~USART_CR1_RXNEIE;
	}

	if(ptrUsartHandler->USART_Config.USART_enableIntRX == USART_INTERRUPT_TX_ENABLE){

		ptrUsartHandler->ptrUSARTx->CR1	&= ~USART_CR1_TXEIE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;
	} else {
		ptrUsartHandler->ptrUSARTx->CR1	&= ~USART_CR1_TXEIE;
	}

	if(ptrUsartHandler->USART_Config.USART_enableIntTX == USART_INTERRUPT_TX_ENABLE || ptrUsartHandler->USART_Config.USART_enableIntRX == USART_INTERRUPT_RX_ENABLE){
		// Matriculamos la interrupción en el NVIC
		if(ptrUsartHandler->ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if (ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if (ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		else {
			__NOP();
		}
	}

	/* Se vuelve a encender las interrupciones globales */
	__enable_irq();

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}
}


void USART1_IRQHandler(void){
	// Evaluacion de la interrupcion en USART1 RX
	if(USART1->SR & USART_SR_RXNE){
		// Se debe guardar la informacion del DR a cual variable? No entendi la funcion con el auxVar
		auxiliar_data_RX = (uint8_t) USART1->DR;
		// Llamar al callback
		callback_USART1_RX();
	}
	else if(USART1->SR & USART_SR_TXE){
		if(dataType == CHAR){
			USART1->DR = dataToSend;
			USART1->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			if(buffer[contador]!='\0'){
				dataToSend = buffer[contador];
				USART1->DR = dataToSend;
				contador++;
			} else {
				USART1->CR1 &= ~USART_CR1_TXEIE;
				contador = 0;
			}
		}
	}
}


void USART2_IRQHandler(void){
	if(USART2->SR & USART_SR_RXNE){
		auxiliar_data_RX = (uint8_t) USART2->DR;
		callback_USART2_RX();
	}
	else if(USART2->SR & USART_SR_TXE){
		if(dataType == CHAR){
			USART2->DR = dataToSend;
			USART2->CR1 &= ~USART_CR1_TXEIE;
		}
		else if(dataType == WORD) {
			if(buffer[contador] != '\0'){
				dataToSend = buffer[contador];
				USART2->DR = dataToSend;
				contador++;
			} else if (buffer[contador] == '\0'){
				USART2->CR1 &= ~USART_CR1_TXEIE;
				contador = 0;
				newData = 1;
			}
		}
	}
}

void USART6_IRQHandler(void){
	if(USART6->SR & USART_SR_RXNE){
		auxiliar_data_RX = (uint8_t) USART6->DR;
		callback_USART2_RX();
	}
	else if(USART6->SR & USART_SR_TXE){
		if(dataType == CHAR){
			USART6->DR = dataToSend;
			USART6->CR1 &= ~USART_CR1_TXEIE;
		}
		else if(dataType == WORD) {
			if(buffer[contador] != '\0'){
				dataToSend = buffer[contador];
				USART6->DR = dataToSend;
				contador++;
			} else if (buffer[contador] == '\0'){
				USART6->CR1 &= ~USART_CR1_TXEIE;
				contador = 0;
				newData = 1;
			}
		}
	}
}

uint8_t get_data_RX (void){
	return auxiliar_data_RX;
}

/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int caracter){
	// Chequeo de registro vacío para el envío
	while(!(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	ptrUsartHandler->ptrUSARTx->DR = caracter;
	return caracter;
}

void writeMsg(USART_Handler_t *ptrUsartHandler, char *word){
	// Chequeo de registro vacío para el envío
	while(!(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	char dataToSend = 0;
	int i = 0;
	while(word[i]){
		dataToSend = word[i];
		writeChar(ptrUsartHandler, dataToSend);
		i++;
	}
}

void interruptWriteChar(USART_Handler_t *ptrUsartHandler, char caracter){
	dataType = CHAR;
	dataToSend = caracter;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;
}

void interruptWriteMsg(USART_Handler_t *ptrUsartHandler, char *word){
	while(newData == 0){
		__NOP();
	}
	dataType = WORD;
	buffer = word;
	contador = 0;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;
	newData = 0;
}

__attribute__ ((weak)) void callback_USART1_RX(void){
	__NOP();
}

__attribute__ ((weak)) void callback_USART2_RX(void){
	__NOP();
}

__attribute__ ((weak)) void callback_USART6_RX(void){
	__NOP();
}

__attribute__ ((weak)) void callback_USART1_TX(void){
	__NOP();
}

__attribute__ ((weak)) void callback_USART2_TX(void){
	__NOP();
}

__attribute__ ((weak)) void callback_USART6_TX(void){
	__NOP();
}

