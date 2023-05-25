/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoya
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
//#include "ExtiDriver.h"
#include "USARTxDriver.h"
//#include "SysTickDriver.h"
//#include "PWMDriver.h"
#include "I2CDriver.h"


/* Definicion de los elementos handler del sistema */

GPIO_Handler_t handlerBlinkyPin =			{0};
GPIO_Handler_t handlerPinTX = 				{0};
GPIO_Handler_t handlerPinRX = 				{0};

BasicTimer_Handler_t handlerBlinkyTimer = 	{0};


USART_Handler_t usart2Comm 	= {0};
uint8_t rxData = 0;
char bufferData[64]= " Accel MPU-6050 testing... ";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;


/* Configuración para el I2C */

GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint16_t i2cBuffer = 0;


#define ACCEL_ADDRESS 	0b0011101; // 0x1D -> Dirección del Accel con Logic_1 (decimal=29)
#define ACCEL_XOUTH_H	59 // 0x3B
#define ACCEL_XOUTH_L	60 // 0x3C
#define ACCEL_YOUTH_H	61 // 0x3D
#define ACCEL_YOUTH_L	62 // 0x3E
#define ACCEL_ZOUTH_H	63 // 0x3F
#define ACCEL_ZOUTH_L	64 // 0x40

#define PWR_MGMT_1		107
#define WHO_AM_I		117



/* Prototipos de las funciones del main*/
void initSystem (void);

/**/


int main(void){

	/*Inicialización de todos los elementos del sistema*/
		initSystem ();

	/* Imprimir mensaje de inicio */
		writeMsg(&usart2Comm, bufferData);

		while(1){

			//Hacemos un "eco" con el valor que nos llega por el serial

			if(rxData != '\0'){
				writeChar(&usart2Comm, rxData);

				if(rxData == 'w'){
					sprintf(bufferData,"WHO_AM_I? (r) ");
					writeMsg(&usart2Comm, bufferData);

					i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
					sprintf(bufferData,"dataRead = 0x%x ", (unsigned int) i2cBuffer);
					writeMsg(&usart2Comm, bufferData);
					rxData = '\0';
				}

				else if(rxData == 'p'){
					sprintf(bufferData,"PWR_MGMT_1 state (r) ");
					writeMsg(&usart2Comm, bufferData);

					i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
					sprintf(bufferData,"dataRead = 0x%x ", (unsigned int) i2cBuffer);
					writeMsg(&usart2Comm, bufferData);
					rxData = '\0';
				}

				else if(rxData == 'r'){
					sprintf(bufferData,"PWR_MGMT_1 reset (w) ");
					writeMsg(&usart2Comm, bufferData);

					i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
					rxData = '\0';
				}

				else if(rxData == 'x'){
					sprintf(bufferData,"Axis X data (r) ");
					writeMsg(&usart2Comm, bufferData);

					uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUTH_L);
					uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUTH_H);
					int16_t AccelX = AccelX_high << 8 | AccelX_low;
					sprintf(bufferData,"AccelX = %d ", (int) AccelX);
					writeMsg(&usart2Comm, bufferData);
					rxData = '\0';
				}
				else if(rxData == 'y'){
					sprintf(bufferData,"Axis Y data (r) ");
					writeMsg(&usart2Comm, bufferData);

					uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUTH_L);
					uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUTH_H);
					int16_t AccelY = AccelY_high << 8 | AccelY_low;
					sprintf(bufferData,"AccelY = %d ", (int) AccelY);
					writeMsg(&usart2Comm, bufferData);
					rxData = '\0';
				}
				else if(rxData == 'z'){
					sprintf(bufferData,"Axis Z data (r) ");
					writeMsg(&usart2Comm, bufferData);

					uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUTH_L);
					uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUTH_H);
					int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
					sprintf(bufferData,"AccelZ = %d ", (int) AccelZ);
					writeMsg(&usart2Comm, bufferData);
					rxData = '\0';
				}
				else{
					rxData = '\0';
				}
			}
		}
}

/**/
void initSystem (void){

	/*Configuracion del BlinkyPin - PA5 para usarlo como LED de estado */
	handlerBlinkyPin.pGPIOx = GPIOH;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerBlinkyPin);

	/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 2500;  // Lanza una interrupción cada 250ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	// Cargamos la configuracion del TIM2
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuracion de la comunicacion serial */
	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerPinRX);


	usart2Comm.ptrUSARTx 						= USART2;
	usart2Comm.USART_Config.USART_baudrate 		= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity 		= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode 			= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX   = USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX   = USART_RX_INTERRUPT_DISABLE;


	USART_Config(&usart2Comm);

// Configuramos los pines sobre los que funciona el I2C1 */
	handlerI2cSL.pGPIOx 										= GPIOB;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinNumber 					= PIN_8;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerI2cSL.GPIO_PinConfig.GPIO_PinAltFunMode 				= AF4;

	GPIO_Config(&handlerI2cSL);


// Configuramos los pines sobre los que funciona el I2C1 */
	handlerI2cSDA.pGPIOx 										= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode 			= AF4;

	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx			= I2C1;
	handlerAccelerometer.modeI2C			= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress		= ACCEL_ADDRESS;

	i2c_config(&handlerAccelerometer);

} // Termina el int_Hardware


//Interrupción lanzada por TIM2 para LED de estado
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Interrupción del botón userButton */

void callback_extInt13 (void){
	__NOP();
}

/* Esta función se ejecuta cada vez que un caracter es recibido por el puerto
 * USART 2 */

void usart2Rx_Callback(void){

	rxData = getRxData();

}




