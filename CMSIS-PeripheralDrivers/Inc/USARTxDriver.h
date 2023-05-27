/*
 * USARTxDriver.h
 *
 *  Created on: Apr 6, 2022
 *      Author: Sebastian Montoya
 */

#include <stdio.h>

#ifndef USARTXDRIVER_H_
#define USARTXDRIVER_H_

#define CHAR 0
#define WORD 1

#define USART_NUM1 0
#define USART_NUM2 1
#define USART_NUM6 2

#define USART_MODE_TX		0
#define USART_MODE_RX		1
#define USART_MODE_RXTX		2
#define USART_MODE_DISABLE	3

#define USART_BAUDRATE_9600		0
#define USART_BAUDRATE_19200	1
#define USART_BAUDRATE_115200	2

#define USART_DATASIZE_8BIT		0
#define USART_DATASIZE_9BIT		1

#define USART_PARITY_NONE	0
#define USART_PARITY_ODD	1
#define USART_PARITY_EVEN	2

#define USART_STOPBIT_1		0
#define USART_STOPBIT_0_5	1
#define USART_STOPBIT_2		2
#define USART_STOPBIT_1_5	3

#define USART_INTERRUPT_RX_NONE 	0
#define USART_INTERRUPT_RX_ENABLE 	1

#define USART_INTERRUPT_TX_NONE		0
#define USART_INTERRUPT_TX_ENABLE	1



/* Estructura para la configuración de la comunicacion:
 * Velocidad (baudrate)
 * Tamaño de los datos
 * Control de errores
 * Bit de parada
 */
typedef struct
{
	uint8_t MCU_frequency;
	uint8_t USART_mode;
	uint8_t USART_baudrate;
	uint8_t USART_datasize;
	uint8_t USART_parity;
	uint8_t USART_stopbits;
	uint8_t USART_enableIntTX;
	uint8_t USART_enableIntRX;
}USART_Config_t;

/*
 * Definicion del Handler para un USART:
 * - Estructura que contiene los SFR que controlan el periferico
 * - Estructura que contiene la configuración especifica del objeto
 * - Buffer de recepcion de datos
 * - Elemento que indica cuantos datos se recibieron
 * - Buffer de transmision de datos
 * - Elemento que indica cuantos datos se deben enviar.
 */
typedef struct
{
	USART_TypeDef	*ptrUSARTx;
	USART_Config_t	USART_Config;
	uint8_t			receptionBuffer[64];
	uint8_t			dataInputSize;
	uint8_t			transmisionBuffer[64];
	uint8_t			dataOutputSize;
}USART_Handler_t;

/* Definición variable para datos */



/* Definicion de callbacks para interrupciones USART */
void callback_USART1_RX(void);
void callback_USART2_RX(void);
void callback_USART6_RX(void);
void callback_USART1_TX(void);
void callback_USART2_TX(void);
void callback_USART6_TX(void);

/* Definicion de los prototipos para las funciones del USART */
uint8_t get_data_RX (void);
void USART_Config(USART_Handler_t *ptrUsartHandler);
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend );
void writeMsg(USART_Handler_t *ptrUsartHandler, char *word);
void interruptWriteChar(USART_Handler_t *ptrUsartHandler, char caracter);
void interruptWriteMsg(USART_Handler_t *ptrUsartHandler, char *word);



#endif /* USARTXDRIVER_H_ */
