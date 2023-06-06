/*
 * funciones.c
 *
 *  Created on: Jun 6, 2023
 *      Author: Sebastian Montoya
 */

#include "funciones.h"





/* Pin A5 y pin H0 */
void inicializacion_Led_Estado(BasicTimer_Handler_t *timerLed,
									GPIO_Handler_t *blinkyLed,
									GPIO_Handler_t *blinkyLedH1){

	RCC->CR &= ~RCC_CR_HSEON;

	// Timer para LEDs de estado usando el LED2 y pin H0
	timerLed->ptrTIMx = TIM2;
	timerLed->TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed->TIMx_Config.TIMx_speed = BITMER_SPEED_16Mhz_100us;
	timerLed->TIMx_Config.TIMx_period = 2500; 			// Tiempo en milisegundos
	timerLed->TIMx_Config.TIMx_interruptEnable = ENABLE; // Activar interrupción
	BasicTimer_Config(timerLed);

	// Controlador de LED2 asignado como led de estado
	blinkyLed->pGPIOx = GPIOA;
	blinkyLed->GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	blinkyLed->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLed->GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLed->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	blinkyLed->GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(blinkyLed);

	// Controlador de pinH0 asignado como led de estado
	blinkyLedH1->pGPIOx = GPIOH;
	blinkyLedH1->GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	blinkyLedH1->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	blinkyLedH1->GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	blinkyLedH1->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(blinkyLedH1);
}

/* Pin A2 y A3 */
void inicializacion_USART2(GPIO_Handler_t *pinTX2, GPIO_Handler_t *pinRX2, USART_Handler_t *usart2){

	// Para realizar transmisión por USB se utilizan los pines PA2 (TX) y PA3 (RX)
	// Inicializacion de PIN A2 con funcion alternativa de USART2
	pinTX2->pGPIOx = GPIOA;
	pinTX2->GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	pinTX2->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinTX2->GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(pinTX2);

	// Inicialización de PIN A3 con función alternativa de USART2
	pinRX2->pGPIOx = GPIOA;
	pinRX2->GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	pinRX2->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinRX2->GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(pinRX2);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	usart2->ptrUSARTx = USART2;
	usart2->USART_Config.USART_mode = USART_MODE_RXTX;
	usart2->USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	usart2->USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	usart2->USART_Config.USART_parity = USART_PARITY_NONE;
	usart2->USART_Config.USART_stopbits = USART_STOPBIT_1;
	usart2->USART_Config.USART_enableIntRX = ENABLE;
	usart2->USART_Config.USART_enableIntTX = ENABLE;
	usart2->USART_Config.MCU_frequency = getPLL();
	USART_Config(usart2);
}

/* Pin A11 y A12 */
void inicializacion_USART6(GPIO_Handler_t *pinTX6, GPIO_Handler_t *pinRX6, USART_Handler_t *usart6){

	// Inicializacion de PIN A11 con funcion alternativa de USART
	pinTX6->pGPIOx = GPIOA;
	pinTX6->GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	pinTX6->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinTX6->GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(pinRX6);

	// Inicialización de PIN A12 con función alternativa de USART
	pinRX6->pGPIOx = GPIOA;
	pinRX6->GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	pinRX6->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinRX6->GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(pinRX6);

	// Inicialización de módulo serial USART2 transmisión + recepción e interrupción RX
	usart6->ptrUSARTx = USART6;
	usart6->USART_Config.USART_mode = USART_MODE_RXTX;
	usart6->USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	usart6->USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	usart6->USART_Config.USART_parity = USART_PARITY_NONE;
	usart6->USART_Config.USART_stopbits = USART_STOPBIT_1;
	usart6->USART_Config.USART_enableIntRX = ENABLE;
	usart6->USART_Config.USART_enableIntTX = ENABLE;
	usart6->USART_Config.MCU_frequency = getPLL();
	USART_Config(usart6);
}

/* Pines dependientes del canal */
void inicializacion_ADC(ADC_Config_t *ptrADC, uint8_t numeroCanales){

	ptrADC.channel[0] = ADC_CHANNEL_0;
	ptrADC.channel[1] = ADC_CHANNEL_1;
	ptrADC.dataAlignment = ADC_ALIGNMENT_RIGHT;
	ptrADC.resolution = ADC_RESOLUTION_10_BIT;
	ptrADC.samplingPeriod = ADC_SAMPLING_PERIOD_15_CYCLES;
	numeroCanales = 2;
	adc_Config_MultiCH(&ptrADC, numeroCanales);
}


void chequear_Comando(char *ptrBuffer_Recepcion, USART_Handler_t *ptrUSART){
//	VA EN EL MAIN sscanf(ptrBuffer_Recepcion,"%s %u %u %s", cmd, &parametro_1, &parametro_2, userMsg);

	if(strcmp(cmd, "help") == 0){
		imprimir_help();
	}
	else if(strcmp(cmd, "test") == 0){

	}
	else {
		interruptWriteMsg(&ptrUSART, "Comando erróneo.");
	}

}

void imprimir_help(USART_Handler_t *ptrUSART){
	interruptWriteMsg(&USART_handler, "Menú de comandos\n");
	interruptWriteMsg(&USART_handler, "1) help - Imprimir este menú\n");
	interruptWriteMsg(&USART_handler, "2) - Control 1 de MCO1 cambiar señal - Elige entre HSI, LSE o PLL\n");
	interruptWriteMsg(&USART_handler, "3) - Control 2 de MCO2 cambiar preescaler - Elige entre \n");
	interruptWriteMsg(&USART_handler, "4) - RTC 1\n");
	interruptWriteMsg(&USART_handler, "5) - RTC 2\n");
	interruptWriteMsg(&USART_handler, "6) - RTC 3\n");
	interruptWriteMsg(&USART_handler, "7) - RTC 4\n");
	interruptWriteMsg(&USART_handler, "8) - ADC 1\n");
	interruptWriteMsg(&USART_handler, "9) - ADC 2\n");
	interruptWriteMsg(&USART_handler, "10) - Acel 1\n");
	interruptWriteMsg(&USART_handler, "11) - Acel 2\n");
}

void test_cmd(uint8_t parametro1, uint8_t parametro2){
	interruptWriteMsg(&USART_handler, "Test CMD\n");

	sprintf(buffer_datos, "numero 1 = %u", parametro1);
	interruptWriteMsg(&USART_handler, buffer_datos);

	sprintf(buffer_datos, "numero 2 = %u", parametro2);
	interruptWriteMsg(&USART_handler, buffer_datos);
}
