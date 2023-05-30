/*
 * Main_principal.c
 *
 *  Created on: May 25, 2023
 *      Author: Sebastian Montoya
 */
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"
#include "AcelerometroDriver.h"
#include "LCDDriver.h"


#define TIMER_80Mhz_100us 	8100
#define TIMER_80Mhz_10us 	810
#define TIMER_16Mhz_100us 	1600

/* Handlers */
// PWM
GPIO_Handler_t pinPWMX_handler ={0};
PWM_Handler_t PWMX_handler = {0};

GPIO_Handler_t pinPWMY_handler ={0};
PWM_Handler_t PWMY_handler = {0};

GPIO_Handler_t pinPWMZ_handler ={0};
PWM_Handler_t PWMZ_handler = {0};

// USART6
GPIO_Handler_t PinTX_handler = {0};
GPIO_Handler_t PinRX_handler = {0};
USART_Handler_t USART2_handler = {0};

GPIO_Handler_t PinTX_U6_handler = {0};
GPIO_Handler_t PinRX_U6_handler = {0};
USART_Handler_t USART6_handler = {0};

// LED de estado
BasicTimer_Handler_t timerLed = {0};
GPIO_Handler_t blinkyLed = {0};

// PLL
PLL_Config_t config_PLL = {0};
system_Clock_data data_Reloj = {0};

// I2C
GPIO_Handler_t pin_timer100Khz = {0};
BasicTimer_Handler_t timer100Khz = {0};

GPIO_Handler_t pinSCL_AXL345 = {0};
GPIO_Handler_t pinSDA_AXL345 = {0};
I2C_Handler_t AXL345 = {0};
 //LCD direccion 0x22
BasicTimer_Handler_t timer_LCD = {0};
GPIO_Handler_t pinSCDA_LCD = {0};
GPIO_Handler_t pinACL_LCD = {0};
I2C_Handler_t LCD_handler = {0};
/* ######## */


/* Variables */
char data_recibida_USART = 0;
uint16_t timer = 0;
uint16_t timer2 = 0;
uint8_t timer_flag = 0;
char usart_buffer[128] = {0};
uint8_t modo_recepcion_data = STANDARD_DATA;

axis_Data_t datos_individuales = {0};
uint16_t posicion_iX = 0;
uint16_t posicion_iY = 0;
uint16_t posicion_iZ= 0;

axis_Data_t datos_muestreo = {0};
uint16_t posicion_muestreo = 0;
uint8_t startTimer = {0};
/* ######### */


/* Funciones inicialización */
void inicializacion_Led_Estado_80Mhz(void);
void inicializacion_pines_USART2(void);
void inicializacion_PWM(void);
void inicializacion_Led_Estado(void);
void inicializacion_pines_I2C(void);
void inicializacion_timer_100Khz(void);
void inicializacion_USART6(void);
void inicializacion_LCD(void);
/* ######################## */

/* Otras funciones */
void print_Clock_Config(void);
uint16_t dutty_calculation(PWM_Handler_t *PWM_handler, float data);
void datos_LCD(void);
void encenderLCD(void);
/* ############### */

int main(void){
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	systemClock_80MHz(&config_PLL);
	config_SysTick_ms(SYSTICK_LOAD_VALUE_80MHz_1ms);
	inicializacion_Led_Estado_80Mhz();
	inicializacion_pines_USART2();
//	inicializacion_USART6();
	inicializacion_pines_I2C();
	inicializacion_timer_100Khz();
	inicializacion_PWM();
	inicializacion_AXL345(&USART2_handler, &AXL345);
	inicializacion_LCD();
	encenderLCD();

	while(1){
		 if(modo_recepcion_data == XYZ_CONSTANT_DATA){
			XYZ_dataset(&USART2_handler,&AXL345, &datos_muestreo, posicion_muestreo);
			print_XYZ_Data(&datos_muestreo, &USART2_handler, posicion_muestreo);
			posicion_muestreo++;
			modo_recepcion_data = STANDARD_DATA;
		}
		else if(modo_recepcion_data == STANDARD_DATA){
			if(data_recibida_USART != '\0'){
				if(data_recibida_USART == 'w'){
					device_ID(&USART2_handler, &AXL345);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'r'){
					read_PowerMode(&USART2_handler, &AXL345);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'x'){
					single_data_X(&USART2_handler, &AXL345, &datos_individuales, posicion_iX);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'y'){
					single_data_Y(&USART2_handler, &AXL345, &datos_individuales, posicion_iY);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'z'){
					single_data_Z(&USART2_handler, &AXL345, &datos_individuales, posicion_iZ);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 's'){
					XYZ_dataset(&USART2_handler,&AXL345, &datos_muestreo,posicion_muestreo);
					print_XYZ_Data(&datos_muestreo, &USART2_handler, posicion_muestreo);
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'c'){
					if(timer_flag == 0){
						timer_flag = 1;
						posicion_muestreo = 0;
					} else {
						timer_flag = 0;
					}
					data_recibida_USART = '\0';
				}
				else if(data_recibida_USART == 'd'){
					data_recibida_USART = '\0';
					posicion_muestreo = 0;
					timer_flag = 2;
				}
				else if(data_recibida_USART == 'g'){
					print_Clock_Config();
					data_recibida_USART = '\0';
				}

				else{
					data_recibida_USART = '\0';
				}

			}

		} else if (modo_recepcion_data == XYZ_2SEC_DATA){
			for(int i = posicion_muestreo; i < BIGDATA_BUFFER; i++){
				XYZ_dataset(&USART2_handler,&AXL345, &datos_muestreo,posicion_muestreo);
			}
			timer++;
			if(timer == 2000){
				timer_flag = 0;
				timer = 0;
				posicion_muestreo = 0;
			}
			modo_recepcion_data = STANDARD_DATA;
		}
		 if(startTimer == 1){
			 datos_LCD();
		 }
	}
}






uint16_t dutty_calculation(PWM_Handler_t *PWM_handler, float data){
	float auxData = fabs(data);
	uint16_t newDutty = 0;
	if(data == 0){
		newDutty = (PWM_handler->config.periodo/2);
	}
	else if(data < 0){
		newDutty = (auxData/20) * (PWM_handler->config.periodo);
	}
	else if(data > 0){
		newDutty = (PWM_handler->config.periodo) * (1/2 + (auxData/20));
	}
	return newDutty;
}

void print_Clock_Config(void){
	systemClock_GetConfig(&data_Reloj);

	sprintf(usart_buffer, "%s \nClock Speed = %d Mhz \nAHB1 = %d \nAPB1 = %d Mhz \nAPB2 = %d \n",
			data_Reloj.clock_Source,data_Reloj.clock_Frequency,data_Reloj.AHB_Frequency,
			data_Reloj.APB1_Frequency, data_Reloj.APB2_Frequency);
	interruptWriteMsg(&USART2_handler, usart_buffer);
}

void update_PWM_signals(void){
	float x_value = raw_data_X(&AXL345);
	float y_value = raw_data_Y(&AXL345);
	float z_value = raw_data_Z(&AXL345);

	uint16_t x_dutty = dutty_calculation(&PWMX_handler, x_value);
	uint16_t y_dutty = dutty_calculation(&PWMY_handler, y_value);
	uint16_t z_dutty = dutty_calculation(&PWMZ_handler, z_value);

	updateDuttyCycle(&PWMX_handler, x_dutty);
	updateDuttyCycle(&PWMY_handler, y_dutty);
	updateDuttyCycle(&PWMZ_handler, z_dutty);
}

void encenderLCD(void){

	LCD_setCursor(&LCD_handler, 0, 0);
	LCD_sendSTR(&LCD_handler, "Eje X = ");
	LCD_setCursor(&LCD_handler, 0, 16);
	LCD_sendSTR(&LCD_handler, "m/s2");
	LCD_setCursor(&LCD_handler, 1, 0);
	LCD_sendSTR(&LCD_handler, "Eje Y = ");
	LCD_setCursor(&LCD_handler, 1, 16);
	LCD_sendSTR(&LCD_handler, "m/s2");
	LCD_setCursor(&LCD_handler, 2, 0);
	LCD_sendSTR(&LCD_handler, "Eje Z = ");
	LCD_setCursor(&LCD_handler, 2, 16);
	LCD_sendSTR(&LCD_handler, "m/s2");
	LCD_setCursor(&LCD_handler, 3, 0);
	LCD_sendSTR(&LCD_handler, "Auxilio :)");

}

void datos_LCD(void){

	if(startTimer == 1){

		float ValorX= raw_data_X(&AXL345);
		sprintf(usart_buffer, "%.2f ", ValorX);
		LCD_setCursor(&LCD_handler, 0, 9);
		LCD_sendSTR(&LCD_handler, usart_buffer);

		float AceleracionY= raw_data_Y(&AXL345);
		sprintf(usart_buffer, "%.2f ", AceleracionY);
		LCD_setCursor(&LCD_handler, 1, 9);
		LCD_sendSTR(&LCD_handler, usart_buffer);

		float AceleracionZ = raw_data_Z(&AXL345);
		sprintf(usart_buffer, "%.2f ", AceleracionZ);
		LCD_setCursor(&LCD_handler, 2, 9);
		LCD_sendSTR(&LCD_handler, usart_buffer);
		LCD_setCursor(&LCD_handler, 3, 0);

		startTimer =! startTimer;
	}

}

/* Centelleo led de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
	timer2++;
	if(timer2 > 4){
		update_PWM_signals();
		timer2=0;
	}
}

/* Interrupción por recepción USART */
void callback_USART2_RX(void){
	data_recibida_USART = get_data_RX();
}

void BasicTimer4_Callback(void){
	startTimer =! startTimer;
}

/* Envio de datos a 1Khz */
void BasicTimer5_Callback(void){
	if(timer_flag == 1){
		modo_recepcion_data = XYZ_CONSTANT_DATA;
	} else if (timer_flag == 2){
		modo_recepcion_data = XYZ_2SEC_DATA;
	} else {
		__NOP();
	}
}

/* Pin A5 y TIM2 */
void inicializacion_Led_Estado_80Mhz(void){
	// Timer para LED de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = TIMER_80Mhz_100us;
	timerLed.TIMx_Config.TIMx_period = 2500; // 250 ms
	timerLed.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
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

/* Pin A5 y TIM2 */
void inicializacion_Led_Estado(void){
	// Timer para LED de estado usando el LED2
	timerLed.ptrTIMx = TIM2;
	timerLed.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLed.TIMx_Config.TIMx_speed = BITMER_SPEED_16Mhz_1ms;
	timerLed.TIMx_Config.TIMx_period = 250; // 250 ms
	timerLed.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE; // Activar interrupción
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
void inicializacion_pines_USART2(void){

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
	USART2_handler.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	USART2_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART2_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART2_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART2_handler.USART_Config.USART_enableIntRX = USART_INTERRUPT_RX_ENABLE;
	USART2_handler.USART_Config.USART_enableIntTX = USART_INTERRUPT_TX_ENABLE;
	USART2_handler.USART_Config.MCU_frequency = config_PLL.final_Frequency; // 16Mhz
	USART_Config(&USART2_handler);
}

void inicializacion_USART6(void){
	PinTX_U6_handler.pGPIOx = GPIOA;
	PinTX_U6_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	PinTX_U6_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinTX_U6_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinTX_U6_handler);

	PinRX_U6_handler.pGPIOx = GPIOA;
	PinRX_U6_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	PinRX_U6_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PinRX_U6_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&PinRX_U6_handler);

	USART6_handler.ptrUSARTx = USART6;
	USART6_handler.USART_Config.USART_mode = USART_MODE_RXTX;
	USART6_handler.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	USART6_handler.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	USART6_handler.USART_Config.USART_parity = USART_PARITY_NONE;
	USART6_handler.USART_Config.USART_stopbits = USART_STOPBIT_1;
	USART6_handler.USART_Config.USART_enableIntRX = USART_INTERRUPT_RX_ENABLE;
	USART6_handler.USART_Config.USART_enableIntTX = USART_INTERRUPT_TX_ENABLE;
	USART6_handler.USART_Config.MCU_frequency = config_PLL.final_Frequency; // 16Mhz
	USART_Config(&USART6_handler);
}

/* Pin B5 y TIM3*/
void inicializacion_PWM(void){
	pinPWMX_handler.pGPIOx = GPIOB;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWMX_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWMX_handler);

	PWMX_handler.ptrTIMx = TIM3;
	PWMX_handler.config.channel = PWM_CHANNEL_1;
	PWMX_handler.config.prescaler = TIMER_80Mhz_100us;
	PWMX_handler.config.periodo = 2000;
	PWMX_handler.config.duttyCicle = 1000;
	pwm_Config(&PWMX_handler);
	startPwmSignal(&PWMX_handler);

	pinPWMY_handler.pGPIOx = GPIOB;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWMY_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWMY_handler);

	PWMY_handler.ptrTIMx = TIM3;
	PWMY_handler.config.channel = PWM_CHANNEL_2;
	PWMY_handler.config.prescaler = TIMER_80Mhz_100us;
	PWMY_handler.config.periodo = 2000;
	PWMY_handler.config.duttyCicle = 1000;
	pwm_Config(&PWMY_handler);
	startPwmSignal(&PWMY_handler);

	pinPWMZ_handler.pGPIOx = GPIOB;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWMZ_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWMZ_handler);

	PWMZ_handler.ptrTIMx = TIM3;
	PWMZ_handler.config.channel = PWM_CHANNEL_3;
	PWMZ_handler.config.prescaler = TIMER_80Mhz_100us;
	PWMZ_handler.config.periodo = 2000;
	PWMZ_handler.config.duttyCicle = 1000;
	pwm_Config(&PWMZ_handler);
	startPwmSignal(&PWMZ_handler);

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
	AXL345.modeI2C       = I2C_MODE_SM;
	AXL345.slaveAddress  = AXL345_ADDRESS;
	AXL345.MCU_frequency = config_PLL.final_Frequency;
	i2c_config(&AXL345);
}

void inicializacion_timer_100Khz(void){
	/* Configuración TIM4 para ser el control del muestreo a 1Khz*/
	timer100Khz.ptrTIMx = TIM5;
	timer100Khz.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timer100Khz.TIMx_Config.TIMx_speed = TIMER_80Mhz_10us;
	timer100Khz.TIMx_Config.TIMx_period = 100; // 1 ms
	timer100Khz.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&timer100Khz);
}

/* Configuración para pines a usar en LCD */
void inicializacion_LCD(void){

	pinACL_LCD.pGPIOx        					  = GPIOB;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_10;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinOType	  = GPIO_OTYPE_OPENDRAIN;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	pinACL_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;
	GPIO_Config(&pinACL_LCD);

	pinSCDA_LCD.pGPIOx        					  = GPIOB;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_3;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinOType	  = GPIO_OTYPE_OPENDRAIN;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	pinSCDA_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF9;
	GPIO_Config(&pinSCDA_LCD);

	timer_LCD.ptrTIMx                          = TIM4;
	timer_LCD.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
	timer_LCD.TIMx_Config.TIMx_speed           = TIMER_80Mhz_100us;
	timer_LCD.TIMx_Config.TIMx_period          = 10000;
	timer_LCD.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&timer_LCD);

	LCD_handler.ptrI2Cx      = I2C2;
	LCD_handler.modeI2C      = I2C_MODE_FM;
	LCD_handler.slaveAddress = 0x22;
	LCD_handler.MCU_frequency = 80;
	i2c_config(&LCD_handler);
	LCD_Init(&LCD_handler);

//	encenderLCD();
}























