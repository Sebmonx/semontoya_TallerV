/*
 * RTCDriver.c
 *
 *  Created on: Jun 4, 2023
 *      Author: ingfisica
 */
#include "RTCDriver.h"
#include <string.h>
#include <stdio.h>


void RTC_config(void){

	/* Encender interfaz de potencia */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/* Modificación de bit que protege contra escritura */
	PWR->CR |= PWR_CR_DBP;

	/* Activar LSE, señal de reloj para RTC */
	RCC->BDCR |= RCC_BDCR_LSEON;

	/* Esperar estabilidad LSE */
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}

	/* Elegir señal de reloj para RTC */
	RCC->BDCR |= RCC_BDCR_RTCSEL_0; // LSE

	/* Encender periferico RTC */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/* Escritura de llaves para desbloquear RTC */
	RTC->WPR = RTC_KEY1;
	RTC->WPR = RTC_KEY2;

	/* Espera que los registros oscuros se sincronicen */
	while(!(RTC->ISR & RTC_ISR_RSF)){
		__NOP() ;
	}

	/* Configuración hora */
	RTC_Time_Change(12, 45, 0);

	/* Configuración fecha */
	RTC_Date_Change(9, 2, 00, MONDAY);
}

void RTC_Time_Change(uint8_t hour, uint8_t minutes, uint8_t seconds){

	/* Desactivar los registros oscuros */
	RTC->CR |= RTC_CR_BYPSHAD;

	/* Permitir modificación de valores en RTC */
	RTC->ISR |= RTC_ISR_INIT;

	/* Esperar a estado de modificación */
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}

	/* Notación 24 horas */
	RTC->CR &= ~RTC_CR_FMT;

	/* Reset */
	RTC->TR = 0;

	uint8_t decenas = 0;
	uint8_t unidades = 0;

	/* Chequeo números ingresados */
	if(hour < 24 && minutes < 60 && seconds < 60){
		/* Codificación BCD y escritura de hora */
		decenas = hour/10;
		RTC->TR |= (decenas & 0x3) << RTC_TR_HT_Pos;

		unidades = hour%10;
		RTC->TR |= (unidades & 0xF) << RTC_TR_HU_Pos;

		/* Codificación BCD y escritura de minutos */
		decenas = minutes/10;
		RTC->TR |= (decenas & 0x7) << RTC_TR_MNT_Pos;

		unidades = minutes%10;
		RTC->TR |= (unidades & 0xF) << RTC_TR_MNU_Pos;

		/* Codificación BCD y escritura de segundos */
		decenas = seconds/10;
		RTC->TR |= (decenas & 0x7) << RTC_TR_ST_Pos;

		unidades = seconds%10;
		RTC->TR |= (unidades & 0xF) << RTC_TR_SU_Pos;
	}


	/* Desactivar estado de inicialización */
	RTC->ISR &= ~RTC_ISR_INIT;

	/* Esperar desactivación de estado de inicialización */
	while(RTC->ISR & RTC_ISR_INIT){
		__NOP();
	}

	/* Encendido de registros oscuros */
	RTC->CR &= ~RTC_CR_BYPSHAD;
}

void RTC_Date_Change(uint8_t date, uint8_t month, uint8_t year,  uint8_t weekday){

	/* Bit para desactivar los registros oscuros */
	RTC->CR |= RTC_CR_BYPSHAD;

	/* Permitir modificación de valores en RTC */
	RTC->ISR |= RTC_ISR_INIT;

	/* Esperar a estado de modificación */
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}

	/* Reset */
	RTC->DR = 0;


	/* Chequeo números ingresados */
	if(month < 13 && date < 32){

		/* Conversión a codificación BCD y escritura de año */
		uint8_t decenas = year/10;
		RTC->DR |= (decenas & 0xF) << RTC_DR_YT_Pos;

		uint8_t unidades = year%10;
		RTC->DR |= (unidades & 0xF) << RTC_DR_YU_Pos;

		/* Conversión a codificación BCD y escritura de mes */
		decenas = year/10;
		RTC->DR |= (decenas & 0x1) << RTC_DR_MT_Pos;

		unidades = year%10;
		RTC->DR |= (unidades & 0xF) << RTC_DR_MU_Pos;

		/* Conversión a codificación BCD y escritura de fecha */
		decenas = date/10;
		RTC->DR |= (decenas & 0xF) << RTC_DR_DT_Pos;

		unidades = date%10;
		RTC->DR |= (unidades & 0xF) << RTC_DR_DU_Pos;
	}


	/* Escritura de día de la semana */
	RTC->DR |= weekday << RTC_DR_WDU_Pos;

	RTC->ISR &= ~RTC_ISR_INIT;

	while(RTC->ISR & RTC_ISR_INIT){
		__NOP();
	}

	RTC->CR &= ~RTC_CR_BYPSHAD;
}

void save_RTC_Data(RTC_Data_t *ptrRTC_DAta){
	uint16_t auxVariable = 0;
	uint8_t decenas = 0;
	uint8_t unidades = 0;

	/* Lectura registro horas y guardado */
	// Las máscaras son para tomar los 6 bits asociados a la hora
	decenas = ((RTC->TR >> RTC_TR_HT_Pos) & 0x3)*10;
	unidades = (RTC->TR >> RTC_TR_HU_Pos) & 0xF;
	ptrRTC_DAta->hour = decenas + unidades;

	/* Lectura registro minutos y guardado */
	// Las máscaras son para tomar los 7 bits asociados a los minutos
	decenas = ((RTC->TR >> RTC_TR_MNT_Pos) & 0x7)*10;
	unidades = (RTC->TR >> RTC_TR_MNU_Pos) & 0xF;
	ptrRTC_DAta->minutes = decenas + unidades;

	/* Lectura registro segundos y guardado */
	// Las máscaras son para tomar los 7 bits asociados a los segundos
	decenas = ((RTC->TR >> RTC_TR_ST_Pos) & 0x7)*10;
	unidades = (RTC->TR >> RTC_TR_SU_Pos) & 0xF;
	ptrRTC_DAta->seconds = decenas + unidades;

	/* Lectura registro año y guardado */
	// Las máscaras son para tomar los 8 bits asociados al año
	decenas = ((RTC->DR >> RTC_DR_YT_Pos) & 0xF)*10;
	unidades = (RTC->DR >> RTC_DR_YU_Pos) & 0xF;
	ptrRTC_DAta->year = decenas + unidades;

	/* Lectura registro mes y guardado */
	// Las máscaras son para tomar los 5 bits asociados al mes
	decenas = ((RTC->DR >> RTC_DR_MT_Pos) & 0x1)*10;
	unidades = (RTC->DR >> RTC_DR_MU_Pos) & 0xF;
	ptrRTC_DAta->month = decenas + unidades;

	/* Lectura registro día y guardado */
	// Las máscaras son para tomar los 6 bits asociados al día
	decenas = ((RTC->DR >> RTC_DR_DT_Pos) & 0x3)*10;
	unidades = (RTC->DR >> RTC_DR_DU_Pos) & 0xF;
	ptrRTC_DAta->date = decenas + unidades;

	/* Lectura registro día de semana y guardado */
	// La máscara es para tomar los 3 bits asociados al día de la semana
	auxVariable = (RTC->DR >> RTC_DR_WDU_Pos) & 0x7;
	switch (auxVariable) {
		case MONDAY:
			strcpy(ptrRTC_DAta->weekday, "LUNES");
			break;
		case TUESDAY:
			strcpy(ptrRTC_DAta->weekday, "MARTES");
			break;
		case WEDNESDAY:
			strcpy(ptrRTC_DAta->weekday, "MIERCOLES");
			break;
		case THURSDAY:
			strcpy(ptrRTC_DAta->weekday, "JUEVES");
			break;
		case FRYDAY:
			strcpy(ptrRTC_DAta->weekday, "VIERNES");
			break;
		case SATURDAY:
			strcpy(ptrRTC_DAta->weekday, "SABADO");
			break;
		case SUNDAY:
			strcpy(ptrRTC_DAta->weekday, "DOMINGO");
			break;
		default:
			break;
	}
}










