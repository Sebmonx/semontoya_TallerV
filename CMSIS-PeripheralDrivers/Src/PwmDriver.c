/*
 * PwmDriver.c
 *
 *  Created on: XXXX , 2022
 *      Author: Sebastian Montoya
 */
#include "PwmDriver.h"
#include <math.h>

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		/* Encendido de señal de reloj TIM2 */
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		/* Encendido de señal de reloj TIM3 */
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		/* Encendido de señal de reloj TIM4 */
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM5){
		/* Encendido de señal de reloj TIM5 */
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	/* Selección de dirección */
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* Reinicio de counter */
	ptrPwmHandler->ptrTIMx->CNT = 0;

	/* Buffer ARR */
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_ARPE;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrPwmHandler->config.channel){

	case PWM_CHANNEL_1:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(0b111 << TIM_CCMR1_OC1M_Pos); /* Limpieza bits */
		ptrPwmHandler->ptrTIMx->CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos); /* Configuracion PWM */

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

		// Activar alta velocidad
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1FE;

		// No afectado por la cosa ETRF
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC1CE;

		break;
	}

	case PWM_CHANNEL_2:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(0b111 << TIM_CCMR1_OC2M_Pos); /* Limpieza bits */
		ptrPwmHandler->ptrTIMx->CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos); /* Configuracion PWM */

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

		// Activar alta velocidad
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2FE;

		// No afectado por la cosa ETRF
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC2CE;
		break;
	}

	case PWM_CHANNEL_3:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~(0b111 << TIM_CCMR2_OC3M_Pos); /* Limpieza bits */
		ptrPwmHandler->ptrTIMx->CCMR2 |= (0b110 << TIM_CCMR2_OC3M_Pos); /* Configuracion PWM */

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;

		// Activar alta velocidad
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3FE;

		// No afectado por la cosa ETRF
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC3CE;
		break;
	}

	case PWM_CHANNEL_4:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;

		// Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~(0b111 << TIM_CCMR2_OC4M_Pos); /* Limpieza bits */
		ptrPwmHandler->ptrTIMx->CCMR2 |= (0b110 << TIM_CCMR2_OC4M_Pos); /* Configuracion PWM */

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;

		// Activar alta velocidad
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4FE;

		// No afectado por la cosa ETRF
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC4CE;
		break;
	}
	default:{
		break;
	}

	/* 6. Activamos la salida seleccionada */
	enableOutput(ptrPwmHandler);

	}// fin del switch-case
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	/* Activación de timer */
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	/* Activación de canal */
	enableOutput(ptrPwmHandler);
}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	/* Desactivación de timer */
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

	/* Desctivación de canal */
	switch(ptrPwmHandler->config.channel){
	case(PWM_CHANNEL_1):{
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;
		break;
	}

	case(PWM_CHANNEL_2):{
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2E;
		break;
	}

	case(PWM_CHANNEL_3):{
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3E;
		break;
	}

	case(PWM_CHANNEL_4):{
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4E;
		break;
	}

	default:{
		__NOP();
		break;
	}
	}
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
		case PWM_CHANNEL_1: {
			// Activamos la salida del canal 1
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
			break;
		}
		case PWM_CHANNEL_2: {
			// Activamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
			break;
		}
		case PWM_CHANNEL_3: {
			// Activamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
			break;
		}
		case PWM_CHANNEL_4: {
			// Activamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
			break;
		}
		default: {
			break;
		}
	}
}

/* 
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	ptrPwmHandler->ptrTIMx->PSC = 0;
	ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.

	ptrPwmHandler->ptrTIMx->ARR = 0;
	/* Carga de período */
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo;

}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
    ptrPwmHandler->config.periodo = (1/newFreq);

	// Llamamos a la función que cambia la frecuencia
	setFrequency(ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
		case PWM_CHANNEL_1:{
			/* Valor conocido */
			ptrPwmHandler->ptrTIMx->CCR1 = 0;
			ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;
			break;
		}
		case PWM_CHANNEL_2:{
			/* Valor conocido */
			ptrPwmHandler->ptrTIMx->CCR2 = 0;
			ptrPwmHandler->ptrTIMx->CCR2= ptrPwmHandler->config.duttyCicle;
			break;
		}
		case PWM_CHANNEL_3:{
			/* Valor conocido */
			ptrPwmHandler->ptrTIMx->CCR3 = 0;
			ptrPwmHandler->ptrTIMx->CCR3 = ptrPwmHandler->config.duttyCicle;
			break;
		}
		case PWM_CHANNEL_4:{
			/* Valor conocido */
			ptrPwmHandler->ptrTIMx->CCR4 = 0;
			ptrPwmHandler->ptrTIMx->CCR4 = ptrPwmHandler->config.duttyCicle;
			break;
		}
		default:{
			__NOP();
			break;
		}
	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
    ptrPwmHandler->config.duttyCicle = newDutty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	setDuttyCycle(ptrPwmHandler);
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



