/*
 * RTCDriver.c
 *
 *  Created on: Jun 4, 2023
 *      Author: ingfisica
 */
#include "RTCDriver.h"
#include <string.h>


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


	/* Escritura de llaves para desbloquear RTC */
	RTC->WPR = RTC_KEY1;
	RTC->WPR = RTC_KEY2;

//	/* Configuración hora */
//	RTC_Time_Change(1, 1, 0);
//
//	/* Configuración fecha */
//	RTC_Date_Change(2000, 2, 9, WEDNESDAY);

	/* Encender periferico RTC */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/* Apagar modificación de valores */
	RTC->ISR &= ~RTC_ISR_INIT;

}

void RTC_Time_Change(uint8_t hour, uint8_t minutes, uint8_t seconds){
	/* Permitir modificación de valores en RTC */
	RTC->ISR |= RTC_ISR_INIT;

//	/* Esperar a estado de modificación */
//	while(!(RTC->ISR & RTC_ISR_INITF)){
//		__NOP();
//	}

	/* Reset */
	RTC->TR = 0;

	/* Inicio de configuraciones para reloj */
	RTC->CR &= ~RTC_CR_FMT; // 24 horas
	RTC->TR &= ~RTC_TR_PM; 	// notación 24 horas

	/* Conversión a codificación BCD y escritura de hora */
	uint8_t auxVariable = binaryToBCD(hour);
	RTC->TR |= auxVariable << RTC_TR_HU_Pos;

	/* Conversión a codificación BCD y escritura de minutos */
	auxVariable = binaryToBCD(minutes);
	RTC->TR |= auxVariable << RTC_TR_MNU_Pos;

	/* Conversión a codificación BCD y escritura de segundos */
	auxVariable = binaryToBCD(seconds);
	RTC->TR |= auxVariable << RTC_TR_SU;

}

void RTC_Date_Change(uint16_t year, uint8_t month, uint8_t date, uint8_t weekday){
	/* Permitir modificación de valores en RTC */
	RTC->ISR |= RTC_ISR_INIT;

//	/* Esperar a estado de modificación */
//	while(!(RTC->ISR & RTC_ISR_INITF)){
//		__NOP();
//	}

	/* Reset */
	RTC->DR = 0;

	/* Conversión a codificación BCD y escritura de año */
	uint8_t auxVariable = binaryToBCD(year);
	RTC->DR |= auxVariable << RTC_DR_YU_Pos;

	/* Conversión a codificación BCD y escritura de año */
	auxVariable = binaryToBCD(month);
	RTC->DR |= auxVariable << RTC_DR_MU_Pos;

	/* Conversión a codificación BCD y escritura de año */
	auxVariable = binaryToBCD(date);
	RTC->DR |= auxVariable << RTC_DR_DU_Pos;

	/* Escritura de día de la semana */
	RTC->DR |= weekday;

}

uint8_t binaryToBCD(uint16_t bin_Value){
	uint8_t m = 0, n = 0;
	uint8_t bcd_Value = bin_Value;

	if(bin_Value >= 100){
		m = bin_Value / 100;
		n = bin_Value % 100;
		bcd_Value = (m << 4) | n;
	}
	else if(bin_Value >= 10){
		m = bin_Value / 10;
		n = bin_Value % 10;

		bcd_Value = (m << 4) | n;
 	}

	return bcd_Value;
}

uint16_t BCDToBinary(uint8_t BCD_value){
	uint8_t m = 0; uint8_t n = 0;
	uint16_t bin_Value = 0;

	m = (BCD_value >> 4) * 10;
	n = BCD_value & 0x0F;

	bin_Value = m + n;

	return bin_Value;
}

void save_RTC_Data(current_RTC_t *ptrRTC_DAta){
	uint16_t auxVariable = 0;

	/* Lectura registro horas y guardado */
	auxVariable = (RTC->TR >> RTC_TR_HU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->hour = auxVariable;

	/* Lectura registro minutos y guardado */
	auxVariable = (RTC->TR >> RTC_TR_MNU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->minutes = auxVariable;

	/* Lectura registro segundos y guardado */
	auxVariable = (RTC->TR >> RTC_TR_SU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->seconds = auxVariable;

	/* Lectura registro año y guardado */
	auxVariable = (RTC->DR >> RTC_DR_YU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->year = auxVariable;

	/* Lectura registro mes y guardado */
	auxVariable = (RTC->DR >> RTC_DR_MU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->month = auxVariable;

	/* Lectura registro día y guardado */
	auxVariable = (RTC->DR >> RTC_DR_DU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	ptrRTC_DAta->date = auxVariable;

	/* Lectura registro día de semana y guardado */
	auxVariable = (RTC->DR >> RTC_DR_WDU_Pos) & 0xFF;
	auxVariable = BCDToBinary(auxVariable);
	switch (auxVariable) {
		case MONDAY:
			strcpy(ptrRTC_DAta->weekday, "MONDAY");
			break;
		case TUESDAY:
			strcpy(ptrRTC_DAta->weekday, "TUESDAY");
			break;
		case WEDNESDAY:
			strcpy(ptrRTC_DAta->weekday, "WEDNESDAY");
			break;
		case THURSDAY:
			strcpy(ptrRTC_DAta->weekday, "THURSDAY");
			break;
		case FRYDAY:
			strcpy(ptrRTC_DAta->weekday, "FRYDAY");
			break;
		case SATURDAY:
			strcpy(ptrRTC_DAta->weekday, "SATURDAY");
			break;
		case SUNDAY:
			strcpy(ptrRTC_DAta->weekday, "SUNDAY");
			break;
		default:
			break;
	}
}










