/*
 * funciones.h
 *
 *  Created on: Jun 6, 2023
 *      Author: Sebastian Montoya
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "RTCDriver.h"






void chequear_Comando(char *ptrBuffer_Recepcion, USART_Handler_t *ptrUSART);
void inicializacion_Led_Estado(BasicTimer_Handler_t *timerLed, GPIO_Handler_t *blinkyLed, GPIO_Handler_t *blinkyLedH1);
void inicializacion_USART2(GPIO_Handler_t *pinTX2, GPIO_Handler_t *pinRX2, USART_Handler_t *usart2);
void inicializacion_USART6(GPIO_Handler_t *pinTX6, GPIO_Handler_t *pinRX6, USART_Handler_t *usart6);



#endif /* FUNCIONES_H_ */
