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
#include "stm32h7xx_hal.h"

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
#define pg_mcu_Pin GPIO_PIN_15
#define pg_mcu_GPIO_Port GPIOI
#define eth_rst_Pin GPIO_PIN_5
#define eth_rst_GPIO_Port GPIOJ
#define eth_int_Pin GPIO_PIN_6
#define eth_int_GPIO_Port GPIOH
#define disp_button_up_Pin GPIO_PIN_7
#define disp_button_up_GPIO_Port GPIOH
#define disp_button_down_Pin GPIO_PIN_8
#define disp_button_down_GPIO_Port GPIOH
#define disp_button_left_Pin GPIO_PIN_9
#define disp_button_left_GPIO_Port GPIOH
#define disp_button_right_Pin GPIO_PIN_10
#define disp_button_right_GPIO_Port GPIOH
#define disp_button_enter_Pin GPIO_PIN_11
#define disp_button_enter_GPIO_Port GPIOH
#define disp_button_escape_Pin GPIO_PIN_12
#define disp_button_escape_GPIO_Port GPIOH
#define display_OnOff_Pin GPIO_PIN_15
#define display_OnOff_GPIO_Port GPIOH
#define display_reset_Pin GPIO_PIN_0
#define display_reset_GPIO_Port GPIOI
#define display_chip_select_Pin GPIO_PIN_1
#define display_chip_select_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
