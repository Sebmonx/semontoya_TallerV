/*
 * acel_sara.c
 *
 *  Created on: May 24, 2023
 *      Author: ingfisica
 */


#include "stm32f4xx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <stdint.h>
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

/* Definción de Variable */
GPIO_Handler_t BlinkyPin = {0};
GPIO_Handler_t UserButton = {0};
GPIO_Handler_t PinTX = {0};
GPIO_Handler_t PinRX = {0};

BasicTimer_Handler_t BlinkyTimer = {0};

USART_Handler_t CommTerminal = {0};

uint8_t rxData = 0;
char bufferData[64] = "Accel MPU-6050 testing...";

/* Configuración para el I2C */
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};

#define ACCEL_ADDRESS          	 0x1D
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55

#define POWER_CTL                45
#define WHO_AM_I                 0

/* Definicion de prototipos de funciones */
void initSystem(void);

int main (void)
{
	initSystem();
//	i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);

	//Imprimir un mensaje de inicio
	writeMsg(&CommTerminal, bufferData);

	while(1){

		//Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&CommTerminal, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&CommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&CommTerminal, bufferData);
				rxData = '\0';
			}
			else if (rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&CommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, POWER_CTL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&CommTerminal, bufferData);
				rxData = '\0';
			}
			else if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&CommTerminal, bufferData);

				i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);
				rxData = '\0';
			}
			else if (rxData == 'x'){
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&CommTerminal, bufferData);

				uint8_t AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsg(&CommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&CommTerminal, bufferData);
				uint8_t AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);
				uint16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsg(&CommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&CommTerminal, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);
				uint16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int)AccelZ);
				writeMsg(&CommTerminal, bufferData);
				rxData = '\0';
			}
			else{
				rxData = '\0';
			}
		}

	}
	return(0);

}

/* Definición de funciones */
void initSystem(void){

	//Configuracion de los handlers
	BlinkyPin.pGPIOx                                 = GPIOA;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber          = PIN_5;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOType           = GPIO_OTYPE_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	BlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	GPIO_Config(&BlinkyPin);

	UserButton.pGPIOx                                 = GPIOC;
	UserButton.GPIO_PinConfig.GPIO_PinNumber          = PIN_13;
	UserButton.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_IN;
	UserButton.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	GPIO_Config(&UserButton);

	//Configuración de la comunicación serial

	//Configuracion de transmision
	PinTX.pGPIOx                                     = GPIOA;
	PinTX.GPIO_PinConfig.GPIO_PinNumber              = PIN_2;
	PinTX.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_ALTFN;
	PinTX.GPIO_PinConfig.GPIO_PinOType				 = GPIO_OTYPE_PUSHPULL;
	PinTX.GPIO_PinConfig.GPIO_PinPuPdControl		 = GPIO_PUPDR_NOTHING;
	PinTX.GPIO_PinConfig.GPIO_PinSpeed				 = GPIO_OSPEED_FAST;
	PinTX.GPIO_PinConfig.GPIO_PinAltFunMode          = AF7;
	GPIO_Config(&PinTX);

	//Configuracion recepción
	PinRX.pGPIOx                                     = GPIOA;
	PinRX.GPIO_PinConfig.GPIO_PinNumber              = PIN_3;
	PinRX.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_ALTFN;
	PinRX.GPIO_PinConfig.GPIO_PinOType				 = GPIO_OTYPE_PUSHPULL;
	PinRX.GPIO_PinConfig.GPIO_PinPuPdControl		 = GPIO_PUPDR_NOTHING;
	PinRX.GPIO_PinConfig.GPIO_PinSpeed				 = GPIO_OSPEED_FAST;
	PinRX.GPIO_PinConfig.GPIO_PinAltFunMode          = AF7;
	GPIO_Config(&PinRX);

	CommTerminal.ptrUSARTx                             = USART2;
	CommTerminal.USART_Config.USART_baudrate           = USART_BAUDRATE_19200;
	CommTerminal.USART_Config.USART_datasize           = USART_DATASIZE_8BIT;
	CommTerminal.USART_Config.USART_parity             = USART_PARITY_NONE;
	CommTerminal.USART_Config.USART_stopbits           = USART_STOPBIT_1;
	CommTerminal.USART_Config.USART_mode               = USART_MODE_RXTX;
	CommTerminal.USART_Config.USART_enableIntRX        = 1;
	CommTerminal.USART_Config.USART_enableIntTX        = 0;
	CommTerminal.USART_Config.MCU_frequency			   = 16;
	USART_Config(&CommTerminal);

	//Configuracion de los timers
	BlinkyTimer.ptrTIMx                              = TIM2;
	BlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP;
	BlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_100us;
	BlinkyTimer.TIMx_Config.TIMx_period              = 2500;
	BlinkyTimer.TIMx_Config.TIMx_interruptEnable     = 1;
	BlinkyTimer.TIMx_Config.APB1_frequency 			 = 16;
	BasicTimer_Config(&BlinkyTimer);

	//Configuración I2C
	I2cSCL.pGPIOx                                    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSCL);

	I2cSDA.pGPIOx                                    = GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSDA);

	Accelerometer.ptrI2Cx                            = I2C1;
	Accelerometer.modeI2C                            = I2C_MODE_FM;
	Accelerometer.slaveAddress                       = ACCEL_ADDRESS;
	i2c_config(&Accelerometer);



}

/* Interrupciones */
void callback_USART2_RX(void){
	//Leemo el valor del registro DR, donde se encuentra almacenado el dato que llega
	// ESto además debe bajar la bandera de la interrupción
	rxData = get_data_RX();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&BlinkyPin);
}




