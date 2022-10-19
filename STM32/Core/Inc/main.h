/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define led_Pin GPIO_PIN_13
#define led_GPIO_Port GPIOC
#define keyMode_Pin GPIO_PIN_0
#define keyMode_GPIO_Port GPIOB
#define keySpeed_Pin GPIO_PIN_1
#define keySpeed_GPIO_Port GPIOB
#define keyDir_Pin GPIO_PIN_2
#define keyDir_GPIO_Port GPIOB
#define keyLigth_Pin GPIO_PIN_10
#define keyLigth_GPIO_Port GPIOB
#define keyProgram_Pin GPIO_PIN_12
#define keyProgram_GPIO_Port GPIOB
#define keyAdd_Pin GPIO_PIN_13
#define keyAdd_GPIO_Port GPIOB
#define keyBrigth_Pin GPIO_PIN_14
#define keyBrigth_GPIO_Port GPIOB
#define keyConfig_Pin GPIO_PIN_15
#define keyConfig_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
