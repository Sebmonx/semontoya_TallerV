/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 3, 2023
 *      Author: semontoya
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED		16000000		//main clock signal value (HSI High Speed Internal)
#define HSE_CLOCK_SPEED		4000000		//main clock signal value (HSI High Speed External)

#define NOP()		asm("NOP")
#define __weak		__attribute__((weak))


//Direcciones memoria Flash y RAM
#define FLASH_BASE_ADDR 	0x08000000u		//memoria del programa 512KB
#define SRAM_BASE_ADDR 		0x20000000u		//memoria RAM 128KB


//Direcciones memoria para perifericos
//En datasheer fig 14
//Mapa de buses

#define APB1_BASE_ADDR 		0x40000000u
#define APB2_BASE_ADDR		0x40010000u
#define AHB1_BASE_ADDR		0x40020000u
#define AHB2_BASE_ADDR		0x50000000u


//Ahora se definen las direcciones de memoria para los perifericos
//descritos en la tabla 1

//Perifericos ABH2
#define USB_OTG_FS_BASE_ADDR		(AHB2_BASE_ADDR + 0x0000u)

//Perifericos ABH1. No esta completa
#define RCC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3800u)
#define GPIOH_BASE_ADDR		(AHB1_BASE_ADDR + 0x1C00u)
#define GPIOE_BASE_ADDR		(AHB1_BASE_ADDR + 0x1000u)
#define GPIOD_BASE_ADDR		(AHB1_BASE_ADDR + 0x0C00u)
#define GPIOC_BASE_ADDR		(AHB1_BASE_ADDR + 0x0800u)
#define GPIOB_BASE_ADDR		(AHB1_BASE_ADDR + 0x0400u)
#define GPIOA_BASE_ADDR		(AHB1_BASE_ADDR + 0x0000u)



//Macros genericos
#define ENABLE 				1
#define DISABLE				0
#define SET					ENABLE
#define CLEAR				DISABLE
#define RESET				DISABLE
#define GPIO_PIN_SET		SET
#define GPIO_PIN_RESET		RESET
#define FLAG_SET			SET
#define FLAG_RESET			RESET
#define I2C_WRITE			0
#define I2C_READ			1



//Estructura que contiene registros del RCC

typedef struct{
	volatile uint32_t CR;			//CLOCK CONTROL
	volatile uint32_t PLLCFGR;		//PLL CONFIG
	volatile uint32_t CFGR;			//CLOCK CONFIG
	volatile uint32_t CIR;			//CLOCK INTERRUPT
	volatile uint32_t AHB1RSTR;		//AHB1 PERIPHERAL RESET
	volatile uint32_t AHB2RSTR; 	//AHB2 PERIPHERAL RESET
	volatile uint32_t reserved0;
	volatile uint32_t reserved1;
	volatile uint32_t APB1RSTR;		//APB1 PERIPHERAL RESET
	volatile uint32_t APB2RSTR;		//APB2 PERIPHERAL RESET
	volatile uint32_t reserved2;
	volatile uint32_t reserved3;
	volatile uint32_t AHB1ENR;		//AHB1 PERIPHERAL CLOCK ENABLE
	volatile uint32_t AHB2ENR;		//AHB2 PERIPHERAL CLOCK ENABLE
	volatile uint32_t reserved4;
	volatile uint32_t reserved5;
	volatile uint32_t APB1ENR; 		//APB1 CLOCK ENABLE
	volatile uint32_t APB2ENR;		//APB2 CLOCK ENABLE
	volatile uint32_t reserved6;
	volatile uint32_t reserved7;
	volatile uint32_t AHB1LPENR;	//AHB1 CLOCK ENABLE LOW POWER
	volatile uint32_t AHB2LPENR;	//AHB2 CLOCK ENABLE	LOW POWER
	volatile uint32_t reserved8;
	volatile uint32_t reserved9;
	volatile uint32_t APB1PENR;		//APB1 CLOCK ENABLE LOW POWER
	volatile uint32_t APB2PENR;		//APB2 CLOCK ENABLE LOW POWER
	volatile uint32_t reserved10;
	volatile uint32_t reserved11;
	volatile uint32_t BDCR; 		//BACKUP DOMAIN CONTROL
	volatile uint32_t CSR; 			//CLOCK CONTROL AND STATUS
	volatile uint32_t reserved12;
	volatile uint32_t reserved13;
	volatile uint32_t SSCGR;		//SPREAD SPECTRUM CLOCK GENERATION
	volatile uint32_t PLLI2SCFGR;	//PLLI2S CONFIG
	volatile uint32_t reserved14;
	volatile uint32_t DCKCFGR;		//DEDICATED CLOCKS CONFIG

} RCC_RegDef_t;

/*Puntero que dirige la memoria definida para RCC a la estructura RCC_RegDef_t*/
#define RCC			((RCC_RegDef_t *) RCC_BASE_ADDR)



//RCC_AHB1ENR, descripcion bit a bit de los perifericos que lo contienen
#define RCC_AHB1ENR_GPIOA_EN		0
#define RCC_AHB1ENR_GPIOB_EN		1
#define RCC_AHB1ENR_GPIOC_EN		2
#define RCC_AHB1ENR_GPIOD_EN		3
#define RCC_AHB1ENR_GPIOE_EN		4
#define RCC_AHB1ENR_GPIOH_EN		7
#define RCC_AHB1ENR_CRCEN			12
#define RCC_AHB1ENR_DMR1_EN			21
#define RCC_AHB1ENR_DMR2_EN			22




/*DESCRIPCION PERIFERICOS GPIOx
 * estructura de datos que definen los perifericos GPIO.
 * No es necesario describir bit a bit los perifericos*/

typedef struct{
	volatile uint32_t MODER;		//PORT MODE REG
	volatile uint32_t OTYPER;		//PORT OUTPUT TYPE
	volatile uint32_t OSPEEDR;		//PORT OUTPUT SPEED
	volatile uint32_t PUPDR; 		//PORT PULL UP/PULL DOWN
	volatile uint32_t IDR;			//PORT INPUT DATA
	volatile uint32_t ODR;			//PORT OUTPUT DATA
	volatile uint32_t BSRR;			//PORT BIT SET/RESET
	volatile uint32_t LCKR;			//PORT CONFIG LOCK
	volatile uint32_t AFRL; 		//ALTERNATE FUNCTION LOW
	volatile uint32_t AFRH;			//ALTERNATE FUNCTION HIGH
}GPIOx_RegDef_t;

/*Puntero que dirige la direccion de memoria a la estructura que define GPIOx
 * definiendo varias veces para cada GPIO existente*/
#define GPIOA				((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB				((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC				((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD				((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE				((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH				((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

/*Valores estandar para las configuraciones*/
/*GPIOx_MODER dos bit por PIN*/
#define GPIO_MODE_IN 		0
#define GPIO_MODE_OUT 		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG 	3

/*GPIOx_OTYPE un bit por PIN*/
#define GPIO_OTYPE_PUSHPULL 	0
#define GPIO_OTYPE_OPENDRAIN	1

/*GPIOx_OPSPEEDR dos bit por PIN*/
#define GPIO_OSPEED_LOW			0
#define GPIO_OSPEED_MEDIUM		1
#define GPIO_OSPEED_FAST		2
#define GPIO_OSPEED_HIGH		3

/*GPIOx_PUPDR dos bit por PIN*/
#define GPIO_PUPDR_NOTHING 		0
#define GPIO_PUPDR_PULLUP 		1
#define GPIO_PUPDR_PULLDOWN		2
#define GPIO_PUPDR_RESERVED		3

/*Definicion de los nombres de los pines*/
#define PIN_0		0
#define PIN_1		1
#define PIN_2		2
#define PIN_3		3
#define PIN_4		4
#define PIN_5		5
#define PIN_6		6
#define PIN_7		7
#define PIN_8		8
#define PIN_9		9
#define PIN_10		10
#define PIN_11		11
#define PIN_12		12
#define PIN_13		13
#define PIN_14		14
#define PIN_15		15

/*Definicion de funciones alternativas*/
#define AF0			0b0000
#define AF1			0b0001
#define AF2			0b0010
#define AF3			0b0011
#define AF4			0b0100
#define AF5			0b0101
#define AF6			0b0110
#define AF7			0b0111
#define AF8			0b1000
#define AF9			0b1001
#define AF10		0b1010
#define AF11		0b1011
#define AF12		0b1100
#define AF13		0b1101
#define AF14		0b1110
#define AF15		0b1111








#endif /* STM32F411XX_HAL_H_ */
