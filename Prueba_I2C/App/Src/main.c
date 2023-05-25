/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sebastian Montoyas
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
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

/* Handlers */
// PWM
GPIO_Handler_t pinPWM_handler ={0};
PWM_Handler_t PWM_handler = {0};

// USART6
GPIO_Handler_t PinTX_handler = {0};
GPIO_Handler_t PinRX_handler = {0};
USART_Handler_t USART2_handler = {0};

// LED de estado
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};

// PLL
PLL_Config_t config_PLL = {0};
system_Clock_data data_Reloj = {0};

// I2C
GPIO_Handler_t pinSCL_handler = {0};
GPIO_Handler_t pinSDA_handler = {0};
I2C_Handler_t I2C_handler = {0};

/* ######## */


/* Variables */
char data_recibida_USART6 = 0;
char data_recibida_USART2 = 0;
char bufferData[64]= " Accel MPU-6050 testing... ";
int timer = 0;
uint16_t i2cBuffer = 0;

/* ######### */


/* Funciones inicialización */
void inicializacion_Led_Estado_16Mhz(void);
void inicializacion_pines_USART(void);
void inicializacion_pines_I2C(void);


/* ######################## */


/* Otras funciones */
/* ############### */

/* Macros */
#define ACCEL_ADDRESS 	0x1D; // 0x1D -> Dirección del Accel con Logic_1 (decimal=29)
#define ACCEL_DATA_X0	50 // 0x32
#define ACCEL_DATA_X1	51 // 0x33
#define ACCEL_DATA_Y0	52 //
#define ACCEL_DATA_Y1	53
#define ACCEL_DATA_Z0	54
#define ACCEL_DATA_Z1	55


#define POWER_CTL		0x2D
#define DEVICE_ID		0x00

/* ###### */


int main(void)
{
	/* Activador coprocesador matemático - FPU */
	//SCB->CPACR |= (0xF << 20);

	inicializacion_Led_Estado_16Mhz();
	inicializacion_pines_I2C();
	inicializacion_pines_USART();

	/* Imprimir mensaje de inicio */
	writeWord(&USART2_handler, bufferData);

	while(1){

		//Hacemos un "eco" con el valor que nos llega por el serial

		if(data_recibida_USART2 != '\0'){
			writeChar(&USART2_handler, data_recibida_USART2);

			if(data_recibida_USART2 == 'w'){
				sprintf(bufferData,"WHO_AM_I? (r) ");
				writeWord(&USART2_handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&I2C_handler, DEVICE_ID);
				sprintf(bufferData,"dataRead = 0x%x ", (unsigned int) i2cBuffer);
				writeWord(&USART2_handler, bufferData);
				data_recibida_USART2 = '\0';
			}

			else if(data_recibida_USART2 == 'p'){
				sprintf(bufferData,"PWR_MGMT_1 state (r) ");
				writeWord(&USART2_handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&I2C_handler, POWER_CTL);
				sprintf(bufferData,"dataRead = 0x%x ", (unsigned int) i2cBuffer);
				writeWord(&USART2_handler, bufferData);
				data_recibida_USART2 = '\0';
			}

			else if(data_recibida_USART2 == 'r'){
				sprintf(bufferData,"PWR_MGMT_1 reset (w) ");
				writeWord(&USART2_handler, bufferData);

				i2c_writeSingleRegister(&I2C_handler, POWER_CTL, 0x00001000);
				data_recibida_USART2 = '\0';
			}

			else if(data_recibida_USART2 == 'x'){
				sprintf(bufferData,"Axis X data (r) ");
				writeWord(&USART2_handler, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_X0);
				uint8_t AccelX_high = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_X1);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData,"AccelX = %d ", (int) AccelX);
				writeWord(&USART2_handler, bufferData);
				data_recibida_USART2 = '\0';
			}
			else if(data_recibida_USART2 == 'y'){
				sprintf(bufferData,"Axis Y data (r) ");
				writeWord(&USART2_handler, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_Y0);
				uint8_t AccelY_high = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_Y1);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData,"AccelY = %d ", (int) AccelY);
				writeWord(&USART2_handler, bufferData);
				data_recibida_USART2 = '\0';
			}
			else if(data_recibida_USART2 == 'z'){
				sprintf(bufferData,"Axis Z data (r) ");
				writeWord(&USART2_handler, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_Z0);
				uint8_t AccelZ_high = i2c_readSingleRegister(&I2C_handler, ACCEL_DATA_Z1);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData,"AccelZ = %d ", (int) AccelZ);
				writeWord(&USART2_handler, bufferData);
				data_recibida_USART2 = '\0';
			}
			else{
				data_recibida_USART2 = '\0';
			}
		}
	}
}

void inicializacion_pines_I2C(void){
	pinSCL_handler.pGPIOx = GPIOB;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinSCL_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&pinSCL_handler);

	pinSDA_handler.pGPIOx = GPIOB;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinSDA_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&pinSDA_handler);

	I2C_handler.ptrI2Cx = I2C1;
	I2C_handler.modeI2C = I2C_MODE_FM;
	I2C_handler.slaveAddress = ACCEL_ADDRESS;
	i2c_config(&I2C_handler);
}

/* Pin A5 y TIM2 */
void inicializacion_Led_Estado_16Mhz(void){
	// Timer para LED de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerLed.TIMx_Config.TIMx_period = 1600; // Tiempo en milisegundos
	timerLed.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	timerLed.TIMx_Config.APB1_frequency = 16; // Frecuencia en bus conectado al timer
	BasicTimer_Config(&timerLed);

	// Controlador de LED2 asignado como led de estado
	blinkyLed.pGPIOx = GPIOA;
	blinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	blinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	blinkyLed.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&blinkyLed);
}

/* Pines A2 y A3 */
void inicializacion_pines_USART(void){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART2
	PinTX_handler.pGPIOx = GPIOA;
	PinTX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinTX_handler);

	// Inicialización de PIN A3 con función alternativa de USART2
	PinRX_handler.pGPIOx = GPIOA;
	PinRX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinRX_handler);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	USART2_handler.ptrUSARTx = USART2;
	USART2_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART2_handler.USART_Config.USART_baudrate = USART_BAUDRATE_9600;
	USART2_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART2_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART2_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART2_handler.USART_Config.USART_enableIntRX = USART_INTERRUPT_RX_ENABLE;
	USART2_handler.USART_Config.USART_enableIntTX = USART_INTERRUPT_TX_NONE;
	USART2_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART2_handler);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	// timer++;
}

/* Interrupción por recepción USART */
void callback_USART2_RX(void){
	data_recibida_USART2 = get_data_RX();
}
