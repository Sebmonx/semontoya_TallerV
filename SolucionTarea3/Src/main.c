/*
 * leds.c
 *
 *  Created on: Apr 6, 2023
 *      Author: ingfisica
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"



/* Variables Display */
	BasicTimer_Handler_t timerDisplay = {0};

	/* Handlers de GPIO display */
	GPIO_Handler_t bit0 = {0};
	GPIO_Handler_t bit1 = {0};
	GPIO_Handler_t bit2 = {0};
	GPIO_Handler_t bit3 = {0};
	GPIO_Handler_t bit4 = {0};
	GPIO_Handler_t bit5 = {0};
	GPIO_Handler_t bit6 = {0};

	/* Arreglo para encendido de LEDs 7 segmentos */
	GPIO_Handler_t bits_7Seg[7] = {0};

	/* Arreglo para encendido de culebrita */
	GPIO_Handler_t pos_snake[12] = {0};

	/* Handlers GPIO transistores */
	GPIO_Handler_t tranDec = {0};
	GPIO_Handler_t tranUni = {0};

/* Variables genericas */
	BasicTimer_Handler_t timerLED2 = {0};
	GPIO_Handler_t handLED2 = {0};
	uint8_t numero = 50;
	uint8_t decena = 0;
	uint8_t unidad = 0;
	uint8_t snake = 0;
	uint8_t modo = 0;
	uint8_t banderaClock = 0;
	uint8_t banderaSwitch = 0;
	uint8_t cont = 0;

/* Variables Encoder */
	GPIO_Handler_t encodCLK = {0};		// Pin para clock de encoder
	EXTI_Config_t exti_encodCLK = {0};	// Estructura para EXTI del clock de encoder
	GPIO_Handler_t encodDT = {0};		// Pin para señal alternativa encoder
	GPIO_Handler_t encodSW = {0}; 		// Pin para botón de cambio de función
	EXTI_Config_t exti_encodSW = {0}; 	// Estructura para EXTI de botón switch


void init_Hardware(void);
uint8_t numero_7Segmentos(uint8_t numero);
void numero_Display(uint8_t numero, GPIO_Handler_t bits[7]);
void culebrita(uint8_t snake, GPIO_Handler_t pos_Snake[12]);



int main(void){
	cont = 0;
	modo = 1;
	init_Hardware();

	while(1){

		/* Función para cambiar el modo de ejecución en el display
		 * utiliza una variable que cambia cada que se presione el
		 * botón del encoder */

		if(banderaSwitch == 1){
			modo = 1;			// modo = 0 para números y modo = 1 para culebrita
		} else if (banderaSwitch == 0){
			modo = 0;
		}


		if(modo == 0){
			/* Función para realizar incremento o decremento del valor que irá
			 * al display restringido entre 0 y 99 leyendo el pin encodDT para
			 * decidir la dirección*/
			if(GPIO_ReadPin(&encodDT) == 1 && banderaClock){		// Dirección
				if(numero == 99){			// Limitante 99
					banderaClock = 0;		// Bajar bandera
				} else {
					numero++;				// Incremento
					banderaClock = 0;		// Bajar bandera
				}
			} else if(!GPIO_ReadPin(&encodDT) == 0 && banderaClock){
				if(numero == 0){			// Limitante 0
					banderaClock = 0;		// Bajar bandera
				} else {
					numero--;				// Decremento
					banderaClock = 0;		// Bajar bandera
				}
			}
		} else if (modo == 1){
			GPIO_WritePin(&bit6, RESET); // Apagado de led g que no se encuentra en el arreglo snake
			/* Función que mueve la culebrita en uno u otro sentido dependiendo
			 * de la señal recibida en el pin del encod DT */
			if(banderaClock == 1 && GPIO_ReadPin(&encodDT) == 1){		// Dirección 1
				if(snake == 11){			// Limitante 12 para regresar a primera posición
					snake = 0;
					banderaClock = 0;		// Bajar bandera
				} else {
					snake++;				// Incremento
					banderaClock = 0;		// Bajar bandera
				}
			} else if(banderaClock == 1 && GPIO_ReadPin(&encodDT) == 0){// Dirección 2

				if(snake == 0){			// Limitante 0 para saltar a posición 12
					snake = 11;
					banderaClock = 0;		// Bajar bandera
				} else {
					snake--;				// Decremento
					banderaClock = 0;		// Bajar bandera
				}
			}
		}

	}
	return 0;
}


void init_Hardware(void){

/* Inicialización de los pines GPIO a utilizar en el display */
	/* LEDS */
	bit0.pGPIOx = GPIOA;
	bit0.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	bit0.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit0.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit0.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit0.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit0);
	GPIO_WritePin(&bit0, SET);

	bit1.pGPIOx = GPIOB;
	bit1.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	bit1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit1.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit1.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit1);
	GPIO_WritePin(&bit1, SET);

	bit2.pGPIOx = GPIOB;
	bit2.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	bit2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit2.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit2);
	GPIO_WritePin(&bit2, SET);

	bit3.pGPIOx = GPIOA;
	bit3.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	bit3.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit3.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit3.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	bit3.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit3);
	GPIO_WritePin(&bit3, SET);

	bit4.pGPIOx = GPIOA;
	bit4.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	bit4.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit4.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit4.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit4.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit4);
	GPIO_WritePin(&bit4, SET);

	bit5.pGPIOx = GPIOB;
	bit5.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	bit5.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit5.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit5.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit5.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit5);
	GPIO_WritePin(&bit5, SET);

	bit6.pGPIOx = GPIOA;
	bit6.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	bit6.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	bit6.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	bit6.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	bit6.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&bit6);
	GPIO_WritePin(&bit6, SET);

	/* Timer para oscilación en transistores */
	timerDisplay.ptrTIMx = TIM3;
	timerDisplay.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	timerDisplay.TIMx_Config.TIMx_period = 10; // Tiempo en milisegundos
	timerDisplay.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerDisplay.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	BasicTimer_Config(&timerDisplay);

	/* Configuración de pines para transistores */
	tranUni.pGPIOx = GPIOB;
	tranUni.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	tranUni.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	tranUni.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	tranUni.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	tranUni.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&tranUni);
	GPIO_WritePin(&tranUni, SET);

	tranDec.pGPIOx = GPIOB;
	tranDec.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	tranDec.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	tranDec.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	tranDec.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	tranDec.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&tranDec);
	GPIO_WritePin(&tranDec, RESET);

	/* Definicion de posiciones en arreglo para números en 7 segmentos */
	bits_7Seg[0] = bit0;
	bits_7Seg[1] = bit1;
	bits_7Seg[2] = bit2;
	bits_7Seg[3] = bit3;
	bits_7Seg[4] = bit4;
	bits_7Seg[5] = bit5;
	bits_7Seg[6] = bit6;

	/* Definicion de posiciones en arreglo para culebrita en 7 segmentos */
	pos_snake[0] = bit0;
	pos_snake[1] = bit0;
	pos_snake[2] = bit5;
	pos_snake[3] = bit4;
	pos_snake[4] = bit3;
	pos_snake[5] = bit4;
	pos_snake[6] = bit5;
	pos_snake[7] = bit1;
	pos_snake[8] = bit2;
	pos_snake[9] = bit3;
	pos_snake[10] = bit2;
	pos_snake[11] = bit1;


	// Timer para LED de estado usando el LED2
	timerLED2.ptrTIMx = TIM2;
	timerLED2.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	timerLED2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	timerLED2.TIMx_Config.TIMx_period = 300; // Tiempo en milisegundos
	timerLED2.TIMx_Config.TIMx_interruptEnable = 1; // Activar interrupción
	BasicTimer_Config(&timerLED2);

	// Controlador de LED2 asignado como led de estado
	handLED2.pGPIOx = GPIOA;
	handLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handLED2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handLED2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handLED2.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handLED2);


/* Inicialización de los pines asociados a las señales entregadas
 * por el encoder */
		// GPIO para clock de encoder
		encodCLK.pGPIOx = GPIOA;
		encodCLK.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
		encodCLK.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodCLK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		// EXTI para interrupción realizada por el clock del encoder
		exti_encodCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
		exti_encodCLK.pGPIOHandler = &encodCLK;
		extInt_Config(&exti_encodCLK);

		// GPIO para señal DT del encoder que determinará la dirección del movimiento
		encodDT.pGPIOx = GPIOB;
		encodDT.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		encodDT.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&encodDT);

		// GPIO para botón del encoder que determinará cambio de función
		encodSW.pGPIOx = GPIOB;
		encodSW.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		encodSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		encodSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		 //EXTI para interrupción realizada por el botón del encoder
		exti_encodSW.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
		exti_encodSW.pGPIOHandler = &encodSW;
		extInt_Config(&exti_encodSW);
}



/* Centelleo de LED de estado */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handLED2);
}

/* Función que convierte número entre 0 y 9 al
 * binario equivalente para el display 7 segmentos
 * Este caso es de cátodo común */
uint8_t numero_7Segmentos(uint8_t numero){

	uint8_t bin = 0;

	switch(numero){

		case 0:{
			bin = 0b0111111;
			break;
		}
		case 1: {
			bin = 0b0000110;
			break;
		}
		case 2: {
			bin = 0b1011011;
			break;
		}
		case 3:{
			bin = 0b1001111;
			break;
		}
		case 4:{
			bin = 0b1100110;
			break;
		}
		case 5:{
			bin = 0b1101101;
			break;
		}
		case 6:{
			bin = 0b1111101;
			break;
		}
		case 7:{
			bin = 0b0000111;
			break;
		}
		case 8:{
			bin = 0b1111111;
			break;
		}
		case 9:{
			bin = 0b1101111;
			break;
		}
		default:{
			__NOP();
		}
	}

	return bin;

}



void BasicTimer3_Callback(void){
	/* Condicional que ejecuta las funciones para los números o la culebrita */
	if(modo == 0){
		if(GPIO_ReadPin(&tranDec)){
			unidad = numero%10;			// Variable para tomar la unidad del número
			numero_Display(unidad, bits_7Seg);	// Enviar unidad al display

			// Alternar transistores
			GPIOxTooglePin(&tranUni);
			GPIOxTooglePin(&tranDec);
		}
		else{
			decena = numero/10;			// Variable para tomar la decena del número
			numero_Display(decena, bits_7Seg);	// Enviar decena al display

			// Alternar transistores
			GPIOxTooglePin(&tranDec);
			GPIOxTooglePin(&tranUni);
		}
	}
	else if(modo == 1){
		culebrita(snake, pos_snake);
	}
}


// Funcion para enviar digito a display
void numero_Display(uint8_t numero, GPIO_Handler_t bits[7]){

	uint8_t auxVar = numero_7Segmentos(numero); // Conversión a binario para display

	for(uint8_t j = 0; j < 7; j++){		// Apagado de todos los leds
			GPIO_WritePin(&bits[j], RESET);
	}

	for(uint8_t i = 0; i < 7; i++){
		/* Encendido de pin asociado al bit del número en binario */
		if(0b1 << i & auxVar){
			GPIO_WritePin(&bits[i], SET);
		} else {
			__NOP();
		}
	}
}


/* Función para elegir transistor y posición del display a encender en el modo culebrita */
void culebrita(uint8_t snake, GPIO_Handler_t pos_Snake[12]){

	for(uint8_t j = 0; j < 12; j++){		// Apagado de todos los leds
		GPIO_WritePin(&pos_Snake[j], RESET);
	}


	switch(snake){
	case 0:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[0], SET);
		break;
	}
	case 1:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[1], SET);
		break;
	}
	case 2:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[2], SET);
		break;
	}
	case 3:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[3], SET);
		break;
	}
	case 4:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[4], SET);
		break;
	}

	case 5:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[5], SET);
		break;
	}
	case 6:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[6], SET);
		break;
	}
	case 7:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[7], SET);
		break;
	}
	case 8:{
		GPIO_WritePin(&tranDec, SET);
		GPIO_WritePin(&tranUni, RESET);
		GPIO_WritePin(&pos_Snake[8], SET);
		break;
	}
	case 9:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[9], SET);
		break;
	}
	case 10:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[10], SET);
		break;
	}
	case 11:{
		GPIO_WritePin(&tranDec, RESET);
		GPIO_WritePin(&tranUni, SET);
		GPIO_WritePin(&pos_Snake[11], SET);
		break;
	}
	default:{
		__NOP();
	}
	}
}

/* Incremento o decremento por encoder */
void callback_extInt10(void){
	banderaClock = 1;
}

/* Función para realizar incremento o decremento del valor que irá
 * al display restringido entre 0 y 99 leyendo el pin encodDT para
 * decidir la dirección*/
void giro_Numero(uint8_t numero, GPIO_Handler_t senal_Dir, uint8_t banderaClock){

	if(GPIO_ReadPin(&senal_Dir) == 1 && banderaClock){		// Dirección
		if(numero == 99){			// Limitante 99
			banderaClock = 0;
		} else {
			numero++;				// Incremento
			banderaClock = 0;
		}
	} else if(!GPIO_ReadPin(&senal_Dir) == 0 && banderaClock){
		if(numero == 0){			// Limitante 0
			banderaClock = 0;
		} else {
			numero--;				// Decremento
			banderaClock = 0;
		}
	}
}

/* Modo de funcion en programa*/
void callback_extInt4(void){
	if(banderaSwitch == 1){
		banderaSwitch = 0;
	}
	else if(banderaSwitch == 0){
		banderaSwitch = 1;
	}
	cont++;
}







