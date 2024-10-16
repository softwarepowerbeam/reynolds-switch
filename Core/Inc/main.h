/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32c0xx_hal.h"

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
#define LAMP2_OUTB_Pin GPIO_PIN_15
#define LAMP2_OUTB_GPIO_Port GPIOC
#define Button_Pin GPIO_PIN_2
#define Button_GPIO_Port GPIOF
#define LAMP2_OUTA_Pin GPIO_PIN_0
#define LAMP2_OUTA_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define UV_IN_Pin GPIO_PIN_2
#define UV_IN_GPIO_Port GPIOA
#define UV_IN_EXTI_IRQn EXTI2_3_IRQn
#define LAMP2_IN_Pin GPIO_PIN_3
#define LAMP2_IN_GPIO_Port GPIOA
#define LAMP2_IN_EXTI_IRQn EXTI2_3_IRQn
#define DIP_IN1_Pin GPIO_PIN_5
#define DIP_IN1_GPIO_Port GPIOA
#define DIP_IN2_Pin GPIO_PIN_6
#define DIP_IN2_GPIO_Port GPIOA
#define UV_OUTB_Pin GPIO_PIN_7
#define UV_OUTB_GPIO_Port GPIOA
#define UV_OUTA_Pin GPIO_PIN_8
#define UV_OUTA_GPIO_Port GPIOA
#define LAMP1_IN_Pin GPIO_PIN_9
#define LAMP1_IN_GPIO_Port GPIOA
#define LAMP1_IN_EXTI_IRQn EXTI4_15_IRQn
#define LAMP1_OUTB_Pin GPIO_PIN_10
#define LAMP1_OUTB_GPIO_Port GPIOA
#define LAMP1_OUTA_Pin GPIO_PIN_6
#define LAMP1_OUTA_GPIO_Port GPIOB
#define DIRLINK_Pin GPIO_PIN_7
#define DIRLINK_GPIO_Port GPIOB
#define DIRLINK_EXTI_IRQn EXTI4_15_IRQn
#define SERIN_Pin GPIO_PIN_14
#define SERIN_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define PYD_DIRECT_LINK_Pin GPIO_PIN_9
#define PYD_DIRECT_LINK_GPIO_Port GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
