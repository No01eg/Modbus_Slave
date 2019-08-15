/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define DIN1_Pin GPIO_PIN_0
#define DIN1_GPIO_Port GPIOA
#define DIN1_EXTI_IRQn EXTI0_IRQn
#define DIN2_Pin GPIO_PIN_1
#define DIN2_GPIO_Port GPIOA
#define DIN2_EXTI_IRQn EXTI1_IRQn
#define DIN3_Pin GPIO_PIN_2
#define DIN3_GPIO_Port GPIOA
#define DIN3_EXTI_IRQn EXTI2_IRQn
#define DIN4_Pin GPIO_PIN_3
#define DIN4_GPIO_Port GPIOA
#define DIN4_EXTI_IRQn EXTI3_IRQn
#define DIN5_Pin GPIO_PIN_4
#define DIN5_GPIO_Port GPIOA
#define DIN5_EXTI_IRQn EXTI4_IRQn
#define DIN6_Pin GPIO_PIN_5
#define DIN6_GPIO_Port GPIOA
#define DIN6_EXTI_IRQn EXTI9_5_IRQn
#define DIN7_Pin GPIO_PIN_6
#define DIN7_GPIO_Port GPIOA
#define DIN7_EXTI_IRQn EXTI9_5_IRQn
#define DIN8_Pin GPIO_PIN_7
#define DIN8_GPIO_Port GPIOA
#define DIN8_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
