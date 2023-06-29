/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body Smontoya
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
//Hardware
#include "OV7670.h"

#include "color_conversion.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define res_QVGA 320*240/2
#define res_CIF 352*288/2
#define res_QCIF 176*144/2
#define res_QQCIF 88*72/2

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

I2C_HandleTypeDef hi2c2;

LPTIM_HandleTypeDef hlptim1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
char uart_Buffer[256];
uint8_t uart_Recieve;

uint8_t conteo_Frames = 0;
uint8_t PWM_flag = 0;
uint8_t DMA_complete_flag = 0;
uint8_t DCMI_Frame_Flag = 0;
uint8_t capturando = 0;

uint32_t image_data[res_CIF/2] = {0};

Camera_settings OV7670_settings={
		QCIF, 		//Resolution
		RGB565,  	//Format
		NORMAL, 	//Effect
		ON,			//AEC
		ON, 		//AGC
		ON, 		//AWB
		OFF,		//Color bar
		OFF,		//vertical flip
		OFF,		//Horizontal flip
		OFF,		//Night mode
		OFF,		//ASC
		ON,			//De-noise
		ON,			//Banding filter
		HISTOGRAM,	//AEC algorithm
		QUARTER_FPS,//Min. fps in night mode
		F_AUTO,		//Auto detect banding freq.
		256, 		//Exposure - 2 bytes
		4, 			//Gain	[0-7]=[1-128]
		160,		//Brightness - byte
		64, 		//Contrast - byte
		80, 		//Saturation - byte
		2,			//Sharpness	- [0-31]
		0,			//De-noise strength - byte
		x16, 		//Gain ceiling
		77, 		//R channel gain - byte
		103, 		//G channel gain - byte
		153			//B channel gain - byte
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DCMI_Init(void);
static void MX_I2C2_Init(void);
static void MX_LPTIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */
//void TransferComplete(DMA_HandleTypeDef *DmaHandle);
void send_Image_UART(uint32_t* image_Data,uint32_t res_Size);
void check_Frame(void);
void send_Frame(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim2){
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		HAL_GPIO_TogglePin(Led_Estado_GPIO_Port, Led_Estado_Pin);

		if(capturando == 1){
			HAL_GPIO_TogglePin(Led_captura_GPIO_Port, Led_captura_Pin);
		}
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim14){
		HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
		 HAL_GPIO_WritePin(Led_Completado_GPIO_Port, Led_Completado_Pin, SET);
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(&huart3, &uart_Recieve, 1);
}

/* No funciona */
//void TransferComplete(DMA_HandleTypeDef *DmaHandle){
//	DMA_complete_flag = DMA_complete_flag + 1;
//}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi){
	DMA_complete_flag = 1;
}

/* No funciona */
//void DMA_XferCpltCallback(DMA_HandleTypeDef *hdcmi_hdma){
//	DMA_complete_flag = DMA_complete_flag + 3;
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_I2C2_Init();
  MX_LPTIM1_Init();
  MX_TIM2_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */

  // Inicialización de timer LED
  HAL_TIM_Base_Start_IT(&htim2);

  // Inicialización de USART para comunicación
  HAL_UART_Receive_IT(&huart3, &uart_Recieve, 1);


  // Apaga cámara
  OV7670_Power(DISABLE);

  /* Funcion que relaciona los perifericos usados con los punteros
  	  creados en el driver */
  OV7670_Init(&hdcmi, &hdma_dcmi, &hi2c2, &hlptim1);

  /* Enciende el reloj que se le va a entregar a la cámara, retorna RST y PWDN
   * a los estados de funcionamiento y realiza un soft reset de los registros
   * en la cámara */
  OV7670_PowerUp();

  sprintf(uart_Buffer, "Camara configurada\n");
  HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);

  /* Realiza configuraciones personalizadas en la cámara, ajustes de fotografía,
   * resolución y funcionamiento */
  OV7670_UpdateSettings(OV7670_settings);
  OV7670_SetFrameRate(XCLK_DIV(1), PLL_x4); // Es posiblemente innecesario porque se va a capturar solo 1 frame cada vez
  HAL_Delay(10);


  /* En teoría en este momento, image_data debería obtener la información de 1 frame */
//  uint32_t image_Data_Full = 0;
//  for(uint32_t i = 0; i < 352*144; i++){
//	  if(image_data[i] == 0){
//		  __NOP();
//	  }
//	  else {
//		  image_Data_Full++;
//	  }
//  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  sprintf(uart_Buffer, "Cada que termina un frame, presione 's' después de guardar los datos.\n");
  HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);
  sprintf(uart_Buffer, "Limpie la pantalla y presione 'i' para iniciar.\n");
  HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);


  while (1)
  {
	 if(uart_Recieve != '\0'){

		 if(uart_Recieve == 'i'){
			 capturando = 1;
		 }

		 else if(uart_Recieve == 'a'){
			 // Iniciar captura
			 OV7670_Start(SNAPSHOT, image_data);

			 // Esperar que llegue el frame completo
			 check_Frame();

			 // Detener captura DCMI cuando el DMA termine
			 OV7670_Stop();

			 /* Transmitir imagen por serial USART */
			 for(uint32_t i = 0; i <  res_QCIF/2; i++){
				  for(int j = 0; j <= 3; j++){
					 uint8_t var = ((image_data[i] >> (8*j)) & 0xFF);
					 HAL_UART_Transmit(&huart3, &var, 1, 100);
				  }
			 }

//			 sprintf(uart_Buffer, "\nFrame enviado.\n");
//			 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);

			 // Controlador para evaluar cuándo el sistema realizó 200 fotos
			 conteo_Frames++;

			 // Inicio de PWM que gira un paso el motor
			 PWM_flag = 1;
			 HAL_TIM_PWM_Start_IT(&htim14, TIM_CHANNEL_1);

//			 sprintf(uart_Buffer, "\nGiro motor.\n");
//			 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);

			 uart_Recieve = '\0';
		 }

		 else if(uart_Recieve == 'e'){
			 sprintf(uart_Buffer, "0D0A");
			 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);
			 uart_Recieve = '\0';
		 }
		 else {
			 uart_Recieve = '\0';
		 }
		 while(capturando == 1 && conteo_Frames < 200){
			 send_Frame();
		 }


//		 while(capturando == 1 && conteo_Frames < 200){
//
//			 // Iniciar captura
//			 OV7670_Start(SNAPSHOT, image_data);
//
//			 // Esperar que llegue el frame completo
//			 check_Frame();
//
//			 // Detener captura DCMI cuando el DMA termine
//			 OV7670_Stop();
//
//			 /* Transmitir imagen por serial USART */
//			 for(uint32_t i = 0; i <  res_QCIF/2; i++){
//				  for(int j = 0; j <= 3; j++){
//					 uint8_t var = ((image_data[i] >> (8*j)) & 0xFF);
//					 HAL_UART_Transmit(&huart3, &var, 1, 100);
//				  }
//			 }
//
////			 sprintf(uart_Buffer, "\nFrame enviado.\n");
////			 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);
//
//			 // Controlador para evaluar cuándo el sistema realizó 200 fotos
//			 conteo_Frames++;
//
//			 // Inicio de PWM que gira un paso el motor
//			 PWM_flag = 1;
//			 HAL_TIM_PWM_Start_IT(&htim14, TIM_CHANNEL_1);
//
////			 sprintf(uart_Buffer, "\nGiro motor.\n");
////			 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);
//
//			 uart_Recieve = '\0';
//
//		 }

	 }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DCMI Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
  hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
  hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
  hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x20404768;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief LPTIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPTIM1_Init(void)
{

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 21600-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2500 - 1 ;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2160-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 25-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 216 -1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 10000;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 5000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|Led_captura_Pin|LD2_Pin|Led_Estado_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Led_Completado_GPIO_Port, Led_Completado_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin Led_captura_Pin LD2_Pin Led_Estado_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|Led_captura_Pin|LD2_Pin|Led_Estado_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : Led_Completado_Pin */
  GPIO_InitStruct.Pin = Led_Completado_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Led_Completado_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void send_Image_UART(uint32_t* image_Data,uint32_t res_Size){
	uint32_t dataAmount = res_Size;
	for(uint32_t i = 0; i <  dataAmount; i++){
		  for(int j = 0; j <= 3; j++){
			 uint8_t var = ((image_Data[i] >> (8*j)) & 0xFF);
			 HAL_UART_Transmit(&huart3, &var, 1, 100);
		  }
	 }
}

void check_Frame(void){
	while(DCMI_Frame_Flag == 0 && HAL_DMA_GetState(&hdma_dcmi) != HAL_DMA_STATE_READY);
	DCMI_Frame_Flag = 0;
}

void send_Frame(void){
	 if(uart_Recieve == 's'){
		 // Apagar pin de completado
		 HAL_GPIO_WritePin(Led_Completado_GPIO_Port, Led_Completado_Pin, RESET);

		 // Reinicio de bandera para chequear nuevo frame
		 DCMI_Frame_Flag = 0;

		 // Iniciar captura
		 OV7670_Start(SNAPSHOT, image_data);

		 // Esperar que llegue el frame completo
		 check_Frame();

		 // Detener captura DCMI cuando el DMA termine
		 OV7670_Stop();

		 /* Transmitir imagen por serial USART */
		 for(uint32_t i = 0; i <  res_QCIF; i++){
			  for(int j = 0; j <= 3; j++){
				 uint8_t var = ((image_data[i] >> (8*j)) & 0xFF);
				 HAL_UART_Transmit(&huart3, &var, 1, 100);
			  }
		 }

//		 sprintf(uart_Buffer, "\nFrame enviado.\n");
//		 HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);

		 // Controlador para evaluar cuándo el sistema realizó 200 fotos
		 conteo_Frames++;

		 // Inicio de PWM que gira un paso el motor
		 PWM_flag = 1;
		 HAL_TIM_PWM_Start_IT(&htim14, TIM_CHANNEL_1);

//		sprintf(uart_Buffer, "\nGiro motor.\n");
//		HAL_UART_Transmit(&huart3, (uint8_t*) uart_Buffer, strlen(uart_Buffer), 100);

		 // Control para evaluar cuándo el sistema realizó 200 fotos
		 conteo_Frames++;
		 uart_Recieve = '\0';
	 }
}


/* No funciona creo */
//void send_Serial_End(void){
//	sprintf(uart_Buffer, "%X %X", 15, 15);
//
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
