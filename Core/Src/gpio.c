/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "usart.h"
#include "math.h"
#include "dataRegs.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = DIN1_Pin|DIN2_Pin|DIN3_Pin|DIN4_Pin 
                          |DIN5_Pin|DIN6_Pin|DIN7_Pin|DIN8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = DIN11_Pin|DIN12_Pin|DIN13_Pin|DIN14_Pin 
                          |DIN15_Pin|DIN16_Pin|DIN9_Pin|DIN10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
//#define ACT_INPUT_MACRO_L (*((u8*)(memMapRegs[0].dataPoint + 10)))
//#define ACT_INPUT_MACRO_H (*((u8*)(memMapRegs[0].dataPoint + 10)))

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	DebounseTime = (*((u16*)(memMapRegs[0].dataPoint + 4)));
	/*GPIO_TypeDef *GPIO = (GPIO_Pin & 0x1ff) ? GPIOA : GPIOB;
	u8 step = (u8)log2(GPIO_Pin);
	if(getStatusBit(step))//getBitFromRegPack(DoutMask, step)
	{
		u32 now_time = HAL_GetTick();
		//проверяем дребезг
		if(now_time - __time_check[step] > 1) //FIx
		{
			//чтение и сравнение входа
			GPIO_PinState state = HAL_GPIO_ReadPin(GPIO, GPIO_Pin) ? 0 : 1;
			uint8_t val = getBitFromRegPack(memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint, step);//((GPIO_Pin & __pins_input_value) ? 1 : 0);
			if(val != state)
			{
				if(state == 1)
					setBitIntoRegPack(memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint, step);//__pins_input_value |= GPIO_Pin;
				else
					resetBitIntoRegPack(memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint, step);//__pins_input_value &= ~GPIO_Pin;
				__time_check[step] = now_time;
			}
		}
		else
		{
			//Тут активация отсчета таймера чтобы потом вычитать наверняка весь регистр входов
		}
	}*/
	/* сразу перенаправляем байт в массив, побитно - долго*/
	/**((u8*)memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint) = ~((GPIOA->IDR) & 0xff) & ACT_INPUT_MACRO_L;
	*((u8*)memMapDigInput[AP_DIGIN_DATA_INPUTS].dataPoint + 1) = ~((GPIOB->IDR >> 8) & 0xff) & ACT_INPUT_MACRO_H;*/

}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
