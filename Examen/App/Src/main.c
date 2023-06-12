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
#include "AcelerometroDriver.h"
#include "arm_math.h"
#include "funciones.h"

// Handlers blinky led
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};
GPIO_Handler_t blinkyLedH1 = {0};

// Variables I2C acelerometro
BasicTimer_Handler_t Timer200hz = {0};

GPIO_Handler_t pinSCL_AXL345 = {0};
GPIO_Handler_t pinSDA_AXL345 = {0};
I2C_Handler_t AXL345_h = {0};

uint16_t datos_EjeZ[1024] = {0};
uint16_t pos_Muestreo = 1024;
uint8_t AXL_Capturar = 0;
uint8_t AXL_Recibir = 0;

// Variables transmisión USART
GPIO_Handler_t PinTX2_h = {0};
GPIO_Handler_t PinRX2_h = {0};

GPIO_Handler_t PinTX6_h = {0};
GPIO_Handler_t PinRX6_h = {0};

USART_Handler_t USART_h = {0};

char data_recibida_USART = 0;
char buffer_datos[256] = "Programa iniciado.\n";
char buffer_Recepcion[256] = {0};
uint8_t contador_Recepcion = 0;
bool string_Completada = false;


// Variables PLL
PLL_Config_t PLL_h = {0};
GPIO_Handler_t MCO1_h = {0};
system_Clock_data clock_Data = {0};

uint8_t MCO1_clock = 0;
uint8_t MCO1_prescaler = 0;
uint8_t frecuencia = 16;
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


// Variables FFT
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusIntFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftsize = 1024;
float32_t datos_FFT[1024];
float32_t datosFFT_T[1024];
PWM_Handler_t testPWM = {0};
GPIO_Handler_t testPinPWM = {0};

// Variables para crear función seno
float32_t sine_value = 0.0;
float32_t sine_ArgValue = 0.0;
#define SINE_SIZE 4096
float32_t fs = 8000;
float32_t f0 = 250;
float32_t dt = 0.0;
float32_t amplitud = 5;
float32_t stopTime = 0.0;
float32_t sineSignal[SINE_SIZE];
float32_t transformed[SINE_SIZE];
float32_t *ptrSineSignal;




int main(void)
{
	inicializacion_Sistema();


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



void BasicTimer2_Callback(){
	GPIOxTooglePin(&blinkyLed);
	GPIOxTooglePin(&blinkyLedH1);

}

void BasicTimer5_Callback(){
	if(pos_Muestreo < 1024){
		AXL_Recibir = 1;
	} else {
		AXL_Capturar = 0;
	}
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
		imprimir_Ayuda();
	}
	else if(strcasecmp(cmd, "ejemplos") == 0){
		imprimir_Ejemplos();
	}
	else if(strcasecmp(cmd, "test") == 0){
		test_CMD();
	}
	else if(strcasecmp(cmd, "relojMCO1") == 0){
		elegir_Reloj_MCO1();
	}
	else if(strcasecmp(cmd, "prescalerMCO1") == 0){
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
	else if(strcasecmp(cmd, "calibrarFrec") == 0){
		calibrar_HSITRIM();
	}
	else if(strcasecmp(cmd, "velocidadADC") == 0){
		velocidad_MuestreoADC();
	}
	else if(strcasecmp(cmd, "iniciarADC") == 0){
		inicio_MuestreoADC();
	}
	else if(strcasecmp(cmd, "fourierAXL") == 0){
		calculo_FFT();
	}
	else if(strcasecmp(cmd, "capturarAXL") == 0){
		inicio_CapturaAXL();
	}
	else {
		interruptWriteMsg(&USART_h, "Comando erróneo.\n");
	}

}


void calculo_FFT(void){
//	for(int i = 0; i < SINE_SIZE; i++){
//		sineSignal[i] = amplitud * arm_sin_f32(2*M_PI*f0*(dt*i));
//	}
//
//	arm_abs_f32(sineSignal, transformed, SINE_SIZE);

	statusIntFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_32,fftsize);

	if(statusIntFFT == ARM_MATH_SUCCESS){
		interruptWriteMsg(&USART_h, "EXITO en inicialización.\n");

		int i = 0;
		int j = 0;

		if(datos_FFT[0] == 0){
			interruptWriteMsg(&USART_h, "Por favor ingresar datos.\n");
		}

		arm_rfft_fast_f32(&config_Rfft_fast_32, datos_FFT, datosFFT_T, ifftFlag);
		arm_abs_f32(datosFFT_T, datos_FFT, fftsize);

		for(i = 1; i < fftsize; i++){
			if(i % 2){
				sprintf(buffer_datos, "%u : %#.6f\n", j, 2*datos_FFT[i]);
				interruptWriteMsg(&USART_h, buffer_datos);
				j++;
			}
		}
		datos_FFT[0] = 0;

	}
	else {
		interruptWriteMsg(&USART_h, "ERROR en inicialización.\n");
	}
}
void inicio_CapturaAXL(void){
	pos_Muestreo = 0;
	AXL_Capturar = 1;
	interruptWriteMsg(&USART_h, "Leyendo datos AXL...\n");
	while(AXL_Capturar == 1){
		if(AXL_Recibir == 1){
			read_Z_data(&AXL345_h, datos_EjeZ, pos_Muestreo);
			datos_FFT[pos_Muestreo] = (float) datos_EjeZ[pos_Muestreo] * (0.0039*9.8);
			pos_Muestreo++;
		}
		AXL_Recibir = 0;
	}

	interruptWriteMsg(&USART_h, "Datos obtenidos.\n");
}


void inicio_MuestreoADC(void){
	ADC_Completo = 0;
	startPwmSignal(&PWM_h);

	while(ADC_Completo != 2){
		__NOP();
	}
	ADC_Contador = 0;


	interruptWriteMsg(&USART_h, "Datos canal 0\n");
	for(int i = 0; i < 256; i++){
		sprintf(buffer_datos, "%d ; %d\n", (i+1), ADC_Data_CH0[i]);
		interruptWriteMsg(&USART_h, buffer_datos);
		delay_ms(1);
	}

	interruptWriteMsg(&USART_h, "Datos canal 1\n");
	for(int i = 0; i < 256; i++){
		sprintf(buffer_datos, "%d ; %d\n", (i+1), ADC_Data_CH1[i]);
		interruptWriteMsg(&USART_h, buffer_datos);
		delay_ms(1);
	}
}

void velocidad_MuestreoADC(void){
	parametro_1 = parametro_1 * 10;

	if(8000 <= parametro_1  && parametro_1 <= 15000){
		updateFrequency(&PWM_h, parametro_1);
		sprintf(buffer_datos, "Frecuencia de muestreo actualizada para %d\n", (parametro_1/10));
		interruptWriteMsg(&USART_h, buffer_datos);
	}
	else {
		interruptWriteMsg(&USART_h, "Elegir una frecuencia adecuada.");
	}
}

void calibrar_HSITRIM(void){
	interruptWriteMsg(&USART_h, "Ajuste de frecuencia.\n");
	interruptWriteMsg(&USART_h, "Para aumentar presione 'u'\n");
	interruptWriteMsg(&USART_h, "Para disminuir presione 'd'\n");
	interruptWriteMsg(&USART_h, "Para salir presione 's'\n");

	while(strcasecmp(cmd, "calibrarFrec") == 0){

		if(data_recibida_USART == 'u'){
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
			cmd[0] = 'f';
		}
		data_recibida_USART = '\0';
	}
	interruptWriteMsg(&USART_h, "Frecuencia calibrada.\n");
}

void cambiar_Hora_RTC(void){
	if(parametro_1 < 24 && parametro_2 < 60 && parametro_3 < 60){
		RTC_Time_Change(parametro_1, parametro_2, parametro_3);
		leer_Hora_RTC();
	}
	else {
		interruptWriteMsg(&USART_h, "Ingrese una hora valida.\n");
	}
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

	if(parametro_2 < 13 && parametro_1 < 32){
		RTC_Date_Change(parametro_1, parametro_2, parametro_3, weekday);
		leer_Fecha_RTC();
	}
	else {
		interruptWriteMsg(&USART_h, "Ingrese una fecha valida.\n");
	}

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
			interruptWriteMsg(&USART_h, "Elegir prescaler con un entero entre 2 y 5.\n");
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
		interruptWriteMsg(&USART_h, "Intenta seleccionando entre HSI, LSE o PLL.\n");
	}
}

void test_CMD(void){
	interruptWriteMsg(&USART_h, "Test CMD\n");

	sprintf(buffer_datos, "numero 1 = %u", parametro_1);
	interruptWriteMsg(&USART_h, buffer_datos);

	sprintf(buffer_datos, "numero 2 = %u", parametro_2);
	interruptWriteMsg(&USART_h, buffer_datos);
}

void imprimir_Ayuda(void){
	interruptWriteMsg(&USART_h, "La escritura de comandos tiene el formato:\n");
	interruptWriteMsg(&USART_h, "'Comando' 'Palabra' 'Número' 'Número' 'Número' + Enter\n\n");
	imprimir_Ejemplos();
	interruptWriteMsg(&USART_h, "Menú de comandos\n");
	interruptWriteMsg(&USART_h, "1) ayuda - Imprimir este menú\n");
	interruptWriteMsg(&USART_h, "2) relojMCO1 - Elige entre HSI, LSE o PLL.\n");
	interruptWriteMsg(&USART_h, "3) prescalerMCO1 - Elige entre 2, 3, 4 o 5.\n");
	interruptWriteMsg(&USART_h, "4) leerHora - Imprime la hora actual del sistema.\n");
	interruptWriteMsg(&USART_h, "5) leerFecha - Imprime la fecha actual del sistema.\n");
	interruptWriteMsg(&USART_h, "6) cambiarHora - Configurar hora a elección HH MM SS.\n");
	interruptWriteMsg(&USART_h, "7) cambiarFecha - Configurar fecha a elección 'DIA DE LA SEMANA' DD MM YY.\n");
	interruptWriteMsg(&USART_h, "8) calibrarFrec - Entra a menú de calibración de frecuencia del sistema.\n");
	interruptWriteMsg(&USART_h, "9) velocidadADC - Elegir velocidad de muestreo ADC entre 800 y 1500 Hz.\n");
	interruptWriteMsg(&USART_h, "10) iniciarADC - Inicia el muestreo de 256 datos.\n");
	interruptWriteMsg(&USART_h, "11) capturarAXL - Inicia la captura de datos del acelerometro.\n");
	interruptWriteMsg(&USART_h, "12) fourierAXL - Realiza la transformada rápida de fourier del acelerometro.\n");
	interruptWriteMsg(&USART_h, "\tEs necesario capturarAXL antes de fourierAXL.\n");

}

void imprimir_Ejemplos(void){
	interruptWriteMsg(&USART_h, "Ejemplos de comandos:\n");
	interruptWriteMsg(&USART_h, "2) relojMCO1 LSE\n");
	interruptWriteMsg(&USART_h, "3) prescalerMCO1 p 2\n");
	interruptWriteMsg(&USART_h, "6) cambiarHora h 23 45 30\n");
	interruptWriteMsg(&USART_h, "7) cambiarFecha Lunes 3 3 23\n\n");
}


void inicializacion_Sistema(void){
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	systemClock_100MHz(&PLL_h);
	frecuencia = getPLL();

	inicializacion_USART2();
	inicializacion_USART6();

	config_SysTick_ms(frecuencia);

	/* Permitir la lectura de frecuencias a través de MCO1 */
	PLL_Frequency_Output(&MCO1_h, MCO1_clock, MCO1_prescaler);

	/* Configuración reloj */
	RTC_config();

	inicializacion_Led_Estado();
	inicializacion_ADC();

	/* PWM para muestreo */
	inicializacion_Trigger_ADC();

	inicializacion_I2C();
	inicializacion_AXL345(&USART_h, &AXL345_h);
	interruptWriteMsg(&USART_h, buffer_datos);
	interruptWriteMsg(&USART_h, "Escriba \"ayuda\" para una guía del programa.\n");
}

/* Pin A5 y pin H0 */
void inicializacion_Led_Estado(void){

	RCC->CR &= ~RCC_CR_HSEON;

	// Timer para LEDs de estado usando el LED2 y pin H1
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

	// Calibración de frecuencia inicial usando el osciloscopio
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
	USART_h.USART_Config.MCU_frequency = frecuencia;
	USART_Config(&USART_h);
}

/* Pin A0 y A1 */
void inicializacion_ADC(void){
	ADC_h.channel[0] = ADC_CHANNEL_0;
	ADC_h.channel[1] = ADC_CHANNEL_1;
	ADC_h.dataAlignment = ADC_ALIGNMENT_RIGHT;
	ADC_h.resolution = ADC_RESOLUTION_12_BIT;
	ADC_h.samplingPeriod[0] = ADC_SAMPLING_PERIOD_480_CYCLES;
	ADC_h.samplingPeriod[1] = ADC_SAMPLING_PERIOD_480_CYCLES;
	adc_Config_MultiCH(&ADC_h, numero_De_Canales);
}

/* Pin B4 - Timer que controla la velocidad de muestreo del ADC */
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

	adc_ExternalTrig();
}

void inicializacion_PWM_Prueba(void){
	testPinPWM.pGPIOx = GPIOD;
	testPinPWM.GPIO_PinConfig.GPIO_PinNumber = PIN_15;
	testPinPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	testPinPWM.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	testPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	testPinPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	testPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&testPinPWM);

	testPWM.ptrTIMx = TIM4;
	testPWM.config.channel = PWM_CHANNEL_4;
	testPWM.config.prescaler = PWM_PRESCALER_100MHz_1us;
	testPWM.config.periodo = 12500;
	testPWM.config.duttyCicle = 500;
	pwm_Config(&testPWM);
	startPwmSignal(&testPWM);
}

void inicializacion_I2C(void){
	// Timer 200 Hz
	Timer200hz.ptrTIMx = TIM5;
	Timer200hz.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	Timer200hz.TIMx_Config.TIMx_speed = BTIMER_SPEED_100Mhz_100us;
	Timer200hz.TIMx_Config.TIMx_period = 50; // Tiempo en milisegundos
	Timer200hz.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&Timer200hz);

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

	AXL345_h.ptrI2Cx       = I2C1;
	AXL345_h.modeI2C       = I2C_MODE_FM;
	AXL345_h.slaveAddress  = AXL345_ADDRESS;
	AXL345_h.MCU_frequency = frecuencia;
	i2c_config(&AXL345_h);
}

















