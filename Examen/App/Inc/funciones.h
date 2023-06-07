


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




void inicializacion_Led_Estado(void);
void inicializacion_ADC(void);
void inicializacion_USART2(void);
void inicializacion_USART6(void);
void chequear_Comando(char *ptrBuffer);
void imprimir_help(void);
void test_CMD(void);
void elegir_Reloj_MCO1(void);
void elegir_PreScaler_MCO1(void);
void leer_Hora_RTC(void);
void leer_Fecha_RTC(void);
void calibrar_HSITRIM(void);
void cambiar_Fecha_RTC(void);
void cambiar_Hora_RTC(void);

#endif /* FUNCIONES_H_ */
