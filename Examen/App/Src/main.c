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
#include <strings.h>
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
#include "funciones.h"

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH1 = {0};

// Variables transmisión USART
GPIO_Handler_t PinTX2_h = {0};
GPIO_Handler_t PinRX2_h = {0};

GPIO_Handler_t PinTX6_h = {0};
GPIO_Handler_t PinRX6_h = {0};

USART_Handler_t USART_h = {0};

char data_recibida_USART = 0;
char buffer_datos[64] = "Test\n";
char buffer_Recepcion[64] = {0};
uint8_t contador_Recepcion = 0;
bool string_Completada = false;


// Variables PLL
PLL_Config_t PLL_h = {0};
GPIO_Handler_t MCO1_h = {0};
system_Clock_data clock_Data = {0};

uint8_t MCO1_clock = 0;
uint8_t MCO1_prescaler = 0;
uint8_t frecuencia = 0;
uint8_t calib_Reloj = 12;

// Variables ADC
ADC_Config_t ADC_h = {0};
uint16_t ADC_Data[2] = {0};
uint16_t ADC_Data_CH0[256] = {0};
uint8_t ADC_Data_CH1[256] = {0};
uint16_t ADC_Contador = 0;
uint8_t numero_De_Canales = 2;
uint8_t ADC_Completo = 0;

GPIO_Handler_t pinPWM_h = {0};
PWM_Handler_t PWM_h = {0};

// Variables comandos
char cmd[64] = {0};
char userMsg[256] = {0};
unsigned int parametro_1 = 0;
unsigned int parametro_2 = 0;
unsigned int parametro_3 = 0;


// Variables RTC
RTC_Data_t RTC_Data = {0};
uint8_t weekday = 0;



int main(void)
{
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);


	systemClock_100MHz(&PLL_h);
	frecuencia = getPLL();
	config_SysTick_ms(frecuencia);
	PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
//	RTC_config();
	inicializacion_Led_Estado();
	inicializacion_USART2();
	inicializacion_USART6();
	inicializacion_ADC();
	inicializacion_Trigger_ADC();
	interruptWriteMsg(&USART_h, buffer_datos);

    /* Loop forever */
	while(1){
		if(data_recibida_USART != '\0'){
			interruptWriteChar(&USART_h, data_recibida_USART);
			buffer_Recepcion[contador_Recepcion] = data_recibida_USART;
			contador_Recepcion++;

			if(data_recibida_USART == '\r'){
				buffer_Recepcion[contador_Recepcion -1] = '\0';
				contador_Recepcion = 0;
				string_Completada = true;
			}

			data_recibida_USART = '\0';
		}

			if(string_Completada){
				chequear_Comando(buffer_Recepcion);
				string_Completada = false;
			}
		}

	return 0;
	}



void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	GPIOxTooglePin(&blinkyLedH1);

}

//void callback_USART2_RX(void){
//	data_recibida_USART = get_data_RX();
//}cambio

void callback_USART6_RX(void){
	data_recibida_USART = get_data_RX();
}

void adcComplete_Callback(void){
	if(ADC_Completo == 0){
		ADC_Data_CH0[ADC_Contador] = getADC();
		ADC_Completo = 1;
	}
	else if(ADC_Completo == 1){
		ADC_Data_CH1[ADC_Contador] = getADC();
		ADC_Completo = 0;
		ADC_Contador++;
	}

	if(ADC_Contador >= 257){
		stopPwmSignal(&PWM_h);
		ADC_Contador = 0;
		ADC_Completo = 2;
	}
}



void chequear_Comando(char *ptrBuffer){

	sscanf(ptrBuffer,"%s %s %u %u %u", cmd, userMsg,
			&parametro_1, &parametro_2, &parametro_3);

	if(strcasecmp(cmd, "ayuda") == 0){
		imprimir_help();
	}
	else if(strcasecmp(cmd, "test") == 0){
		test_CMD();
	}
	else if(strcasecmp(cmd, "MCO1Reloj") == 0){
		elegir_Reloj_MCO1();
	}
	else if(strcasecmp(cmd, "MCO1Prescaler") == 0){
		elegir_PreScaler_MCO1();
	}
	else if(strcasecmp(cmd, "leerHora") == 0){
		leer_Hora_RTC();
	}
	else if(strcasecmp(cmd, "leerFecha") == 0){
		leer_Fecha_RTC();
	}
	else if(strcasecmp(cmd, "cambiarFecha") == 0){
		cambiar_Fecha_RTC();
	}
	else if(strcasecmp(cmd, "cambiarHora") == 0){
		cambiar_Hora_RTC();
	}
	else if(strcasecmp(cmd, "calibrarReloj") == 0){
		calibrar_HSITRIM();
	}
	else if(strcasecmp(cmd, "muestreoADC") == 0){
		parametro_1 = parametro_1 * 10;

		if(8000 <= parametro_1  && parametro_1 <= 15000){
			updateFrequency(&PWM_h, parametro_1);
			sprintf(buffer_datos, "Frecuencia de muestreo actualizada a %d\n", parametro_1);
			interruptWriteMsg(&USART_h, buffer_datos);
		}
		else {
			interruptWriteMsg(&USART_h, "Elegir una frecuencia adecuada.");
		}
	}
	else if(strcasecmp(cmd, "iniciarADC") == 0){
		ADC_Completo = 0;
		startPwmSignal(&PWM_h);

		while(ADC_Completo != 2){
			__NOP();
		}
		ADC_Contador = 0;


		interruptWriteMsg(&USART_h, "Datos canal 0\n");
		for(int i = 0; i < 256; i++){
			sprintf(buffer_datos, "%d : %d\n", (i+1), ADC_Data_CH0[i]);
			interruptWriteMsg(&USART_h, buffer_datos);
			delay_ms(1);
		}

		interruptWriteMsg(&USART_h, "Datos canal 1\n");
		for(int i = 0; i < 256; i++){
			sprintf(buffer_datos, "%d : %d\n", (i+1), ADC_Data_CH1[i]);
			interruptWriteMsg(&USART_h, buffer_datos);
			delay_ms(1);
		}




	}
	else {
		interruptWriteMsg(&USART_h, "Comando erróneo.\n");
	}

}

void calibrar_HSITRIM(void){
	data_recibida_USART = 'c';
	interruptWriteMsg(&USART_h, "Ajuste de reloj\n");
	interruptWriteMsg(&USART_h, "Para aumentar presione 'u'\n");
	interruptWriteMsg(&USART_h, "Para disminuir presione 'd'\n");
	interruptWriteMsg(&USART_h, "Para salir presione 's'\n");

	while(strcasecmp(cmd, "calibrarReloj") == 0){

		if(data_recibida_USART == 'u'){
			data_recibida_USART = 'c';
			calib_Reloj++;
			if(calib_Reloj == 31){
				interruptWriteMsg(&USART_h, "No puede incrementar más el ajuste.\n");
				calib_Reloj = 31;
			}
			RCC->CR &= ~RCC_CR_HSITRIM;
			RCC->CR |= (calib_Reloj & 0x1F) << RCC_CR_HSITRIM_Pos;
			sprintf(buffer_datos, "HSITRIM ajustado a %d\n", calib_Reloj);
			interruptWriteMsg(&USART_h, buffer_datos);
		}
		else if(data_recibida_USART == 'd'){
			data_recibida_USART = 'c';
			calib_Reloj--;
			if(calib_Reloj == 0){
				interruptWriteMsg(&USART_h, "No puede disminuir más el ajuste.\n");
				calib_Reloj = 0;
			}
			RCC->CR &= ~RCC_CR_HSITRIM;
			RCC->CR |= (calib_Reloj & 0x1F) << RCC_CR_HSITRIM_Pos;
			sprintf(buffer_datos, "HSITRIM ajustado a %d\n", calib_Reloj);
			interruptWriteMsg(&USART_h, buffer_datos);
		}

		if(data_recibida_USART == 's'){
			break;
		}
	}
}

void cambiar_Hora_RTC(void){
	RTC_Time_Change(parametro_1, parametro_2, parametro_3);
	leer_Hora_RTC();
}

void cambiar_Fecha_RTC(void){

	if(strcasecmp(userMsg, "lunes") == 0){
		weekday = 1;
	}
	else if(strcasecmp(userMsg, "martes") == 0){
		weekday = 2;
	}
	else if(strcasecmp(userMsg, "miercoles") == 0){
		weekday = 3;
	}
	else if(strcasecmp(userMsg, "jueves") == 0){
		weekday = 4;
	}
	else if(strcasecmp(userMsg, "viernes") == 0){
		weekday = 5;
	}
	else if(strcasecmp(userMsg, "sabado") == 0){
		weekday = 6;
	}
	else if(strcasecmp(userMsg, "domingo") == 0){
		weekday = 7;
	}

	RTC_Date_Change(parametro_1, parametro_2, parametro_3, weekday);
	leer_Fecha_RTC();
}

void leer_Fecha_RTC(void){
	save_RTC_Data(&RTC_Data);
	sprintf(buffer_datos, "%d/%d/%d %s\n.",RTC_Data.date, RTC_Data.month,
			RTC_Data.year, RTC_Data.weekday);
	interruptWriteMsg(&USART_h, buffer_datos);
}

void leer_Hora_RTC(void){
	save_RTC_Data(&RTC_Data);
	sprintf(buffer_datos, "%d:%d:%d H\n.",RTC_Data.hour, RTC_Data.minutes, RTC_Data.seconds);
	interruptWriteMsg(&USART_h, buffer_datos);

}

void elegir_PreScaler_MCO1(void){
	switch (parametro_1) {
		case 2:
			MCO1_prescaler = 2;
			PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
			interruptWriteMsg(&USART_h, "Prescaler 2 seleccionado para MCO1.\n");
			break;

		case 3:
			MCO1_prescaler = 3;
			PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
			interruptWriteMsg(&USART_h, "Prescaler 3 seleccionado para MCO1.\n");
			break;

		case 4:
			MCO1_prescaler = 4;
			PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
			interruptWriteMsg(&USART_h, "Prescaler 4 seleccionado para MCO1.\n");
			break;

		case 5:
			MCO1_prescaler = 5;
			PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
			interruptWriteMsg(&USART_h, "Prescaler 5 seleccionado para MCO1.\n");
			break;

		default:
			interruptWriteMsg(&USART_h, "Intenta seleccionando el prescaler con un entero entre 2 y 5.\n");
			break;
	}
}

void elegir_Reloj_MCO1(void){
	if(strcasecmp(userMsg, "hsi") == 0){
		MCO1_clock = HSI_CLOCK;
		PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
		interruptWriteMsg(&USART_h, "HSI seleccionado para MCO1.\n");
	}
	else if(strcasecmp(userMsg, "lse") == 0){
		MCO1_clock = LSE_CLOCK;
		PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
		interruptWriteMsg(&USART_h, "LSE seleccionado para MCO1.\n");
	}
	else if(strcasecmp(userMsg, "pll") == 0){
		MCO1_clock = PLL_CLOCK;
		PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);
		interruptWriteMsg(&USART_h, "PLL seleccionado para MCO1.\n");
	}
	else {
		interruptWriteMsg(&USART_h, "Intenta seleccionando entre hsi, lse o pll.\n");
	}
}

void test_CMD(void){
	interruptWriteMsg(&USART_h, "Test CMD\n");

	sprintf(buffer_datos, "numero 1 = %u", parametro_1);
	interruptWriteMsg(&USART_h, buffer_datos);

	sprintf(buffer_datos, "numero 2 = %u", parametro_2);
	interruptWriteMsg(&USART_h, buffer_datos);
}

void imprimir_help(void){
	interruptWriteMsg(&USART_h, "Menú de comandos\n");
	interruptWriteMsg(&USART_h, "1) help - Imprimir este menú\n");
	interruptWriteMsg(&USART_h, "2) - Control 1 de MCO1 cambiar señal - Elige entre HSI, LSE o PLL\n");
	interruptWriteMsg(&USART_h, "3) - Control 2 de MCO2 cambiar preescaler - Elige entre \n");
	interruptWriteMsg(&USART_h, "4) - RTC 1\n");
	interruptWriteMsg(&USART_h, "5) - RTC 2\n");
	interruptWriteMsg(&USART_h, "6) - RTC 3\n");
	interruptWriteMsg(&USART_h, "7) - RTC 4\n");
	interruptWriteMsg(&USART_h, "8) - ADC 1\n");
	interruptWriteMsg(&USART_h, "9) - ADC 2\n");
	interruptWriteMsg(&USART_h, "10) - Acel 1\n");
	interruptWriteMsg(&USART_h, "11) - Acel 2\n");
}

/* Pin A5 y pin H0 */
void inicializacion_Led_Estado(void){

	RCC->CR &= ~RCC_CR_HSEON;

	// Timer para LEDs de estado usando el LED2 y pin H0
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

	// Controlador de pinH0 asignado como led de estado
	blinkyLedH1.pGPIOx = GPIOH;
	blinkyLedH1.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	blinkyLedH1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLedH1.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLedH1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&blinkyLedH1);

	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (calib_Reloj & 0x1F) << RCC_CR_HSITRIM_Pos;
}

/* Pin A2 y A3 */
void inicializacion_USART2(void){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART2
	PinTX2_h.pGPIOx = GPIOA;
	PinTX2_h.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	PinTX2_h.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX2_h.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinTX2_h);

	// Inicialización de PIN A3 con función alternativa de USART2
	PinRX2_h.pGPIOx = GPIOA;
	PinRX2_h.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	PinRX2_h.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX2_h.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&PinRX2_h);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	USART_h.ptrUSARTx = USART2;
	USART_h.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_h.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	USART_h.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_h.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_h.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_h.USART_Config.USART_enableIntRX = ENABLE;
	USART_h.USART_Config.USART_enableIntTX = ENABLE;
	USART_h.USART_Config.MCU_frequency = frecuencia;
	USART_Config(&USART_h);
}

/* Pin A11 y A12 */
void inicializacion_USART6(void){

	// Inicializacion de PIN A11 con funcion alternativa de USART
	PinTX6_h.pGPIOx = GPIOA;
	PinTX6_h.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	PinTX6_h.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX6_h.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinTX6_h);

	// Inicialización de PIN A12 con función alternativa de USART
	PinRX6_h.pGPIOx = GPIOA;
	PinRX6_h.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	PinRX6_h.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX6_h.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinRX6_h);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	USART_h.ptrUSARTx = USART6;
	USART_h.USART_Config.USART_mode = USART_MODE_RXTX;
	USART_h.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	USART_h.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART_h.USART_Config.USART_parity = USART_PARITY_NONE;
	USART_h.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART_h.USART_Config.USART_enableIntRX = ENABLE;
	USART_h.USART_Config.USART_enableIntTX = ENABLE;
	USART_h.USART_Config.MCU_frequency = 100;
	USART_Config(&USART_h);
}

/* Pin A0 y A1 */
void inicializacion_ADC(void){
	ADC_h.channel[0] = ADC_CHANNEL_0;
	ADC_h.channel[1] = ADC_CHANNEL_1;
	ADC_h.dataAlignment = ADC_ALIGNMENT_RIGHT;
	ADC_h.resolution = ADC_RESOLUTION_10_BIT;
	ADC_h.samplingPeriod[0] = ADC_SAMPLING_PERIOD_480_CYCLES;
	ADC_h.samplingPeriod[1] = ADC_SAMPLING_PERIOD_480_CYCLES;
	adc_Config_MultiCH(&ADC_h, numero_De_Canales);

	adc_ExternalTrig();
}

/* Pin B4 */
void inicializacion_Trigger_ADC(void){
	pinPWM_h.pGPIOx = GPIOB;
	pinPWM_h.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	pinPWM_h.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWM_h.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWM_h.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWM_h.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWM_h.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWM_h);

	PWM_h.ptrTIMx = TIM3;
	PWM_h.config.channel = PWM_CHANNEL_1;
	PWM_h.config.prescaler = PWM_PRESCALER_100MHz_1us;
	PWM_h.config.periodo = 66;
	PWM_h.config.duttyCicle = 33;
	pwm_Config(&PWM_h);
//	startPwmSignal(&PWM_h);
}
