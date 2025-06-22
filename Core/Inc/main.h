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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DI2_Pin GPIO_PIN_2
#define DI2_GPIO_Port GPIOE
#define DI3_Pin GPIO_PIN_3
#define DI3_GPIO_Port GPIOE
#define DI4_Pin GPIO_PIN_4
#define DI4_GPIO_Port GPIOE
#define DI5_Pin GPIO_PIN_5
#define DI5_GPIO_Port GPIOE
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOC
#define SW2_Pin GPIO_PIN_1
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_2
#define SW3_GPIO_Port GPIOC
#define SW4_Pin GPIO_PIN_3
#define SW4_GPIO_Port GPIOC
#define MEM_CS_Pin GPIO_PIN_4
#define MEM_CS_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_5
#define USER_LED_GPIO_Port GPIOC
#define DO0_Pin GPIO_PIN_0
#define DO0_GPIO_Port GPIOB
#define DO1_Pin GPIO_PIN_1
#define DO1_GPIO_Port GPIOB
#define DO2_Pin GPIO_PIN_2
#define DO2_GPIO_Port GPIOB
#define RS485CTRL_Pin GPIO_PIN_12
#define RS485CTRL_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOD
#define LCD_LED_Pin GPIO_PIN_15
#define LCD_LED_GPIO_Port GPIOA
#define BUZZ_Pin GPIO_PIN_3
#define BUZZ_GPIO_Port GPIOD
#define T_IQR_Pin GPIO_PIN_6
#define T_IQR_GPIO_Port GPIOD
#define T_IQR_EXTI_IRQn EXTI9_5_IRQn
#define T_CS_Pin GPIO_PIN_6
#define T_CS_GPIO_Port GPIOB
#define DI0_Pin GPIO_PIN_0
#define DI0_GPIO_Port GPIOE
#define DI1_Pin GPIO_PIN_1
#define DI1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
