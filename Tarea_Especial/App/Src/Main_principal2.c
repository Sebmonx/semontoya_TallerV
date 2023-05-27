/*
 * Main_principal.c
 *
 *  Created on: May 25, 2023
 *      Author: Sebastian Montoya
 */

#include <stdint.h>
#include <string.h>
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"
#include "AcelerometroDriver.h"


#define TIMER_80Mhz_100us 8100
#define TIMER_16Mhz_100us 1600

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
GPIO_Handler_t pin_timer100Khz = {0};
BasicTimer_Handler_t timer100Khz = {0};
GPIO_Handler_t pinSCL_AXL345 = {0};
GPIO_Handler_t pinSDA_AXL345 = {0};
I2C_Handler_t AXL345 = {0};
/* ######## */


/* Variables */
char data_recibida_USART = 0;
int timer = 0;
uint8_t timer_flag = 0;
char usart_buffer[64];
uint8_t modo_recepcion_data = STANDARD_DATA;

axis_Data_t datos_individuales;
uint16_t posicion_iX;
uint16_t posicion_iY;
uint16_t posicion_iZ;

axis_Data_t datos_muestreo;
uint16_t posicion_muestreo;

/* ######### */


/* Funciones inicialización */
void inicializacion_Led_Estado_80Mhz(void);
void inicializacion_pines_USART2(void);
void inicializacion_PWM(void);
void inicializacion_Led_Estado(void);
void inicializacion_pines_I2C(void);
void inicializacion_timer_100Khz(void);

/* ######################## */

/* Otras funciones */
/* ############### */

int main(void){
	/* Activador coprocesador matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	inicializacion_Led_Estado();
	inicializacion_pines_USART2();
	inicializacion_pines_I2C();
	inicializacion_timer_100Khz();
	inicializacion_AXL345(&USART2_handler, &AXL345, &timer100Khz, &pin_timer100Khz);

	while(1){
		 if(modo_recepcion_data == XYZ_CONSTANT_DATA){
			XYZ_dataset(&USART2_handler,&AXL345, &datos_muestreo,posicion_muestreo);
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
					timer_flag = 2;
					posicion_muestreo = 0;
					data_recibida_USART = '\0';
				}
			}

		} else if (modo_recepcion_data == XYZ_2SEC_DATA){
			XYZ_dataset(NULL,&AXL345, &datos_muestreo,posicion_muestreo);
			if(timer == 2000){
				timer_flag = 0;
				timer = 0;
				print_All_Data(&USART2_handler,&datos_muestreo,posicion_muestreo);
			}
			modo_recepcion_data = STANDARD_DATA;
		}
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
	timerLed.TIMx_Config.TIMx_speed = TIMER_80Mhz_100us;
	timerLed.TIMx_Config.TIMx_period = 2500; // 250 ms
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

/* Pin B5 y TIM3*/
void inicializacion_PWM(void){
	pinPWM_handler.pGPIOx = GPIOB;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pinPWM_handler.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&pinPWM_handler);

	PWM_handler.ptrTIMx = TIM3;
	PWM_handler.config.channel = PWM_CHANNEL_2;
	PWM_handler.config.periodo = 20000;
	PWM_handler.config.prescaler = config_PLL.final_Frequency; /* Incremento cada microsegundo */
	PWM_handler.config.duttyCicle = 0;
	pwm_Config(&PWM_handler);

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

	AXL345.ptrI2Cx              = I2C1;
	AXL345.modeI2C              = I2C_MODE_FM;
	AXL345.slaveAddress			= AXL345_ADDRESS;
	AXL345.MCU_frequency		= config_PLL.final_Frequency;
	i2c_config(&AXL345);
}

void inicializacion_timer_100Khz(void){
	/* Configuración TIM4 para ser el control del muestreo a 1Khz*/
	timer100Khz.ptrTIMx = TIM5;
	timer100Khz.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timer100Khz.TIMx_Config.TIMx_speed = 1600;
	timer100Khz.TIMx_Config.TIMx_period = 10; // 1 ms
	timer100Khz.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&timer100Khz);
}

/* Centelleo led de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&blinkyLed);
}

/* Interrupción por recepción USART */
void callback_USART2_RX(void){
	data_recibida_USART = get_data_RX();
}

/* Envio de datos a 1Khz */
void BasicTimer5_Callback(void){
	if(timer_flag == 1){
		modo_recepcion_data = XYZ_CONSTANT_DATA;
	} else if (timer_flag == 2){
		modo_recepcion_data = XYZ_2SEC_DATA;
		timer++;
	} else {
		__NOP();
	}
}


























