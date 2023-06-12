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
#include "AcelerometroDriver.h"
#include "AdcDriver.h"

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH0 = {0};

// Variables transmisión USART
GPIO_Handler_t PinTX2_handler = {0};
GPIO_Handler_t PinRX2_handler = {0};

GPIO_Handler_t PinTX6_handler = {0};
GPIO_Handler_t PinRX6_handler = {0};

USART_Handler_t USART_handler = {0};

char data_recibida_USART = 0;
char buffer_datos[128] = "Test\n";

// I2C
GPIO_Handler_t pin_timer100Khz = {0};
BasicTimer_Handler_t timer100Khz = {0};

GPIO_Handler_t pinSCL_AXL345 = {0};
GPIO_Handler_t pinSDA_AXL345 = {0};
I2C_Handler_t AXL345 = {0};

axis_Data_t datos_muestreo = {0};
uint16_t pos_Muestreo = 0;

// Funciones de inicialización
void inicializacion_Led_Estado(void);
void inicializacion_USART2(void);
void inicializacion_pines_I2C(void);


int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	inicializacion_Led_Estado();
	inicializacion_USART2();
	inicializacion_pines_I2C();
	inicializacion_AXL345(&USART_handler, &AXL345);

    /* Loop forever */
	while(1){
		if(data_recibida_USART != '\0'){
			if(data_recibida_USART == 'w'){
				device_ID(&USART_handler, &AXL345);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 'r'){
				read_PowerMode(&USART_handler, &AXL345);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 'x'){
				single_data_X(&USART_handler, &AXL345, &datos_muestreo, pos_Muestreo);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 'y'){
				single_data_Y(&USART_handler, &AXL345, &datos_muestreo, pos_Muestreo);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 'z'){
				single_data_Z(&USART_handler, &AXL345, &datos_muestreo, pos_Muestreo);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 's'){
				XYZ_dataset(&USART_handler,&AXL345, &datos_muestreo,pos_Muestreo);
				print_XYZ_Data(&datos_muestreo, &USART_handler, pos_Muestreo);
				data_recibida_USART = '\0';
			}
			else if(data_recibida_USART == 't'){
//				float prueba = read_XYZ_data(&AXL345, &datos_muestreo);
//				sprintf(buffer_datos, "%.2f", prueba);
//				interruptWriteMsg(&USART_handler, buffer_datos);
				data_recibida_USART = '\0';
			}
		}
	}

	return 0;
}


/* Leds de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	GPIOxTooglePin(&blinkyLedH0);
}

/* Recepción por interrupción */
void callback_USART2_RX(void){
	data_recibida_USART = get_data_RX();
}


/* Pin A5 y pin H0 */
void inicializacion_Led_Estado(void){

	RCC->CR &= ~RCC_CR_HSEON;

	// Timer para LEDs de estado usando el LED2 y pin H1
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BTIMER_SPEED_16Mhz_100us;
	timerLed.TIMx_Config.TIMx_period = 2500; 			// 250 ms
	timerLed.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&timerLed);

	// Controlador de LED2 asignado como led de estado
	blinkyLed.pGPIOx = GPIOA;
	blinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	blinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	blinkyLed.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&blinkyLed);

	// Controlador de pinH0 asignado como led de estado
	blinkyLedH0.pGPIOx = GPIOH;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLedH0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&blinkyLedH0);
}

void inicializacion_USART2(void){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART
	PinTX2_handler.pGPIOx = GPIOA;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinTX2_handler);

	// Inicialización de PIN A3 con función alternativa de USART
	PinRX2_handler.pGPIOx = GPIOA;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX2_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinRX2_handler);

	// Inicialización de módulo serial USART transmisión + recepción e interrupción RXTX
	USART_handler.ptrUSARTx = USART2;
	USART_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_handler.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	USART_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_handler.USART_Config.USART_enableIntRX = ENABLE;
	USART_handler.USART_Config.USART_enableIntTX = ENABLE;
	USART_handler.USART_Config.MCU_frequency = 16;
	USART_Config(&USART_handler);
}

/* Pines B8 y B9 + I2C1 */
void inicializacion_pines_I2C(void){
	//Configuración I2C
	pinSCL_AXL345.pGPIOx                                    = GPIOB;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	pinSCL_AXL345.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&pinSCL_AXL345);

	pinSDA_AXL345.pGPIOx                                    = GPIOB;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	pinSDA_AXL345.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&pinSDA_AXL345);

	AXL345.ptrI2Cx       = I2C1;
	AXL345.modeI2C       = I2C_MODE_FM;
	AXL345.slaveAddress  = AXL345_ADDRESS;
	AXL345.MCU_frequency = 16;
	i2c_config(&AXL345);
}
