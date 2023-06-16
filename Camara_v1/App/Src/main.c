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

#define CAM_ADDRESS 0x42


// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
//GPIO_Handler_t blinkyLedH1 = {0};

// Handlers cámara
GPIO_Handler_t xclk_pin = {0};
PWM_Handler_t xclk_pwm = {0};

I2C_Handler_t camI2C = {0};
GPIO_Handler_t cam_SCL = {0};
GPIO_Handler_t cam_SDA = {0};

// Handlers PLL
PLL_Config_t PLL_h = {0};
GPIO_Handler_t MCO_h = {0};



void inicializacion_Led_Estado(void);
void inicializacion_xclk(void);
void inicializacion_camI2C(void);


int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	systemClock_100MHz(&PLL_h);
	PLL_Frequency_Output(&MCO_h, PLL_CLOCK, 5);
	inicializacion_Led_Estado();
	inicializacion_xclk();
	config_SysTick_ms(100);






	/* Loop forever */

	while(1){
		i2c_readSingleRegister(&camI2C, 0x0A);
		delay_ms(500);

	}

	return 0;
}

void BasicTimer2_Callback(){
	GPIOxTooglePin(&blinkyLed);
}

void inicializacion_Led_Estado(void){

	// Timer para LEDs de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BTIMER_SPEED_100Mhz_100us;
	timerLed.TIMx_Config.TIMx_period = 25000; // Tiempo en milisegundos
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

//	// Controlador de pinH0 asignado como led de estado
//	blinkyLedH1.pGPIOx = GPIOH;
//	blinkyLedH1.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
//	blinkyLedH1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	blinkyLedH1.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
//	blinkyLedH1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	GPIO_Config(&blinkyLedH1);

}

void inicializacion_xclk(void){

	xclk_pin.pGPIOx = GPIOB;
	xclk_pin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	xclk_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	xclk_pin.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	xclk_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	xclk_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	xclk_pin.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&xclk_pin);

	xclk_pwm.ptrTIMx = TIM3;
	xclk_pwm.config.channel = PWM_CHANNEL_1;
	xclk_camara(&xclk_pwm);

}

void inicializacion_camI2C(void){
	cam_SCL.pGPIOx = GPIOB;
	cam_SCL.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	cam_SCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	cam_SCL.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_OPENDRAIN;
	cam_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	cam_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	cam_SCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&cam_SCL);

	cam_SDA.pGPIOx = GPIOB;
	cam_SDA.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	cam_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	cam_SDA.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_OPENDRAIN;
	cam_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	cam_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	cam_SDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&cam_SDA);

	camI2C.ptrI2Cx = I2C1;
	camI2C.modeI2C = I2C_MODE_SM;
	camI2C.slaveAddress = CAM_ADDRESS;
	camI2C.MCU_frequency = 100;
	i2c_config(&camI2C);


}














