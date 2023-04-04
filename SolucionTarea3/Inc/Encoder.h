/*
 * Encoder.h
 *
 *  Created on: Apr 3, 2023
 *      Author: ingfisica
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

void init_Encoder(void);
void giro_Numero(uint8_t numero, GPIO_Handler_t senal_Dir);
void cambio_modo(uint8_t modo);


#endif /* ENCODER_H_ */
