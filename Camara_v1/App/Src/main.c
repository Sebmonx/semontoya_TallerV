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

// Timer para dma
BasicTimer_Handler_t T2_h;
PWM_Handler_t T3_h;


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
	inicializacion_camI2C();
	inicializacion_xclk(); // Pwm para entregar señal de reloj
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

void T1_init(void){
	// Señal de reloj
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	TIM1->PSC = 0; // Prescaler
	TIM1->CR1 &= ~TIM_CR1_DIR; // Dirección de conteo
	TIM1->ARR = 65535; // Período
//	TIM1->CR1 &= ~TIM_CR1_CKD; // División de reloj entre timer y filtros de sampling
//	TIM1->RCR &= ~TIM_RCR_REP; // Genera un evento al terminar el conteo

	/* Modo slave */
	TIM1->SMCR |= 0b100 << TIM_SMCR_SMS_Pos; // Activar el modo reset
	TIM1->SMCR |= 0b101 << TIM_SMCR_TS_Pos; // Selecciona la entrada del timer que se sincroniza con el counter

	/* Modo maestro */
	TIM1->CR2 &= ~TIM_CR2_MMS;
	TIM1->SMCR |= TIM_SMCR_MSM;

	/* Configuración de modo entrada */
	TIM1->CCER &= ~TIM_CCER_CC1P;
	TIM1->CCMR1 |= 0b01 << TIM_CCMR1_CC1S_Pos;
	TIM1->CCMR1 &= ~TIM_CCMR1_IC1PSC;
}

void T2_init(void){
	// Timer para LEDs de estado usando el LED2
	T2_h.ptrTIMx = TIM2;
	T2_h.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	T2_h.TIMx_Config.TIMx_speed = 0;
	T2_h.TIMx_Config.TIMx_period = 4294967295; // Tiempo en milisegundos
	BasicTimer_Config(&T2_h);

	/* Modo slave */
	TIM2->SMCR |= 0b111 << TIM_SMCR_SMS_Pos;
	TIM2->SMCR &= ~TIM_SMCR_TS;

	/* Modo maestro */
	TIM2->CR2 &= ~TIM_CR2_MMS;
	TIM2->SMCR &= ~TIM_SMCR_MSM;

	/* Modo entrada */
	TIM2->CCER |= 0b01 << TIM_CCER_CC1P_Pos;
	TIM2->CCMR1 |= 0b01 << TIM_CCMR1_CC1S_Pos;
	TIM2->CCMR1 &= ~TIM_CCMR1_IC1PSC;
}

/* Valio madre porque ya habia otra mas mejor */
void T3_init(void){
	// Señal de reloj
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = 0; // Prescaler
	TIM3->CR1 &= ~TIM_CR1_DIR; // Dirección de conteo
	TIM3->ARR = 7; // Período
//	TIM3->CR1 &= ~TIM_CR1_CKD; // División de reloj entre timer y filtros de sampling
//	TIM3->RCR &= ~TIM_RCR_REP; // Genera un evento al terminar el conteo

	/* Modo maestro */
	TIM3->CR2 &= ~TIM_CR2_MMS;
	TIM3->SMCR &= ~TIM_SMCR_MSM;

	/* Configuración de modo pwm */
	TIM3->CCMR1 |= 0b110 << TIM_CCMR1_OC1M_Pos;
	TIM3->CCR1 = 5;
	TIM3->CCER &= ~TIM_CCER_CC1P;
	TIM3->CCMR1 |= TIM_CCMR1_OC1FE;
	TIM3->CCER |= TIM_CCER_CC1E;

}













