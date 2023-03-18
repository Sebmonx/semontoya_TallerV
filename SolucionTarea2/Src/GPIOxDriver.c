#include "GPIOxDriver.h"

/*Se está creando una función que permita ir paso a paso sobre las configuraciones de los perifericos
en el orden necesario para culminar en un cambio de comportamiento en el periférico.*/

void GPIO_Config(GPIO_Handler_t *pGPIOHandler){
    //Variable para paso a paso
    uint32_t auxConfig = 0;
    uint32_t auxPosition = 0;

    //1. Activar el periferico
    //Verificamos para cada uno de los GPIOx
    if(pGPIOHandler-> pGPIOx == GPIOA){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOA_EN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOB){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOB_EN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOC){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOC_EN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOD){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOD_EN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOE){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOE_EN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOH){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOH_EN);
    }

    /*INICIO DE CONFIGURACION
    2. CONFIGURAR GPIOx_MODER*/

    //Se crea variable leyendo y moviendo el valor pin number veces
    auxConfig = (pGPIOHandler ->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Limpiar bits especificos del registro (0b00) usando una mascara
    pGPIOHandler-> pGPIOx-> MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Cargar la config al MODER
    pGPIOHandler-> pGPIOx->MODER |= auxConfig;

    //3. CONFIGURAR GPIOx_OTYPER
    //Leer y mover
    auxConfig = (pGPIOHandler-> GPIO_PinConfig.GPIO_PinOType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Limpiar
    pGPIOHandler-> pGPIOx-> OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Cargar la config al OTYPER
    pGPIOHandler-> pGPIOx-> OTYPER |= auxConfig;

    //4. CONFIGURAR OSPEED
    auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

    //5. CONFIGURAR PUPD
    auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

    //6. CONFIGURAR FUNCIONES ALTERNATIVAS
    if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

        if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
            //Registro AFRL que controla PIN_0 a PIN_7
            auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;
            pGPIOHandler->pGPIOx->AFRL &= ~(0b111 << auxPosition); //Limpiar
            pGPIOHandler->pGPIOx->AFRL |=  (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition); //Escribir
        } else {
        //Registro AFRH, para PIN_8 a PIN_15
        auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);
        pGPIOHandler->pGPIOx->AFRH &= ~(0b111 << auxPosition); //Limpiar
        pGPIOHandler->pGPIOx->AFRH |=  (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition); //Escribir
        }
    }
}

void GPIO_WritePin(GPIO_Handler_t * pPinHandler, uint8_t newState){
    if(newState == SET){
        pPinHandler->pGPIOx->BSRR |= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
    } else {
        pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
    }

}

uint32_t GPIO_ReadPin(GPIO_Handler_t * pPinHandler){
    uint32_t pinValue = 0; //Variable auxiliar
    pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber); //Lectura en posicion requerida
    return pinValue;
}

void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){

	pPinHandler->pGPIOx->ODR ^= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	/**Encender o apagar el periferico puede ser
	 * apagar o encender la señal de reloj (esto no modifica el valor del óutput, no sirve)
	 * ¿cambiar de input a output? (es un cambio extremo que igual no cambia el valor)
	 * ¿enviar a pull up o pull down? (no es)
	 * si es output cambiar la salida, es decir modificar el output register
	 *
	**/
}

void GPIOxTooglePin1Segundo(GPIO_Handler_t *pPinHandler){
	// Delay de 1 segundo
	for(uint32_t i = 0; i < 1250000; i++){
		NOP();
	}
	// Encendido o apagado de PIN
	pPinHandler->pGPIOx->ODR ^= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
}


