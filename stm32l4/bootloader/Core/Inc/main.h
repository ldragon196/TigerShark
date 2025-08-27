/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FL_CS_Pin GPIO_PIN_9
#define FL_CS_GPIO_Port GPIOB
#define WIFI_EN_Pin GPIO_PIN_6
#define WIFI_EN_GPIO_Port GPIOC
#define WAKE_PW_EN_Pin GPIO_PIN_6
#define WAKE_PW_EN_GPIO_Port GPIOG
#define WIFI_PW_EN_Pin GPIO_PIN_0
#define WIFI_PW_EN_GPIO_Port GPIOG
#define MAIN_SW_Pin GPIO_PIN_5
#define MAIN_SW_GPIO_Port GPIOA
#define FL_HOLD_Pin GPIO_PIN_15
#define FL_HOLD_GPIO_Port GPIOB
#define FL_WP_Pin GPIO_PIN_14
#define FL_WP_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_11
#define LED_BLUE_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_0
#define LED_RED_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_1
#define LED_GREEN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
