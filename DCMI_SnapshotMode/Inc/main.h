/**
  ******************************************************************************
  * @file    DCMI/DCMI_SnapshotMode/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f769i_eval.h"
#include "stm32f769i_eval_lcd.h"
#include "stm32f769i_eval_sram.h"
#include "stm32f769i_eval_camera.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* LCD Frame Buffer address */

#define LCD_FRAME_BUFFER                  0xC0000000 /* LCD Frame buffer of size 800x480 in ARGB8888 */

#define CAMERA_FRAME_BUFFER               0xC0177000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

