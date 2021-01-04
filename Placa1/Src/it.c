/*
 * it.c
 *
 *  Created on: 28 dic. 2020
 *      Author: Andrés Zapata
 */

#include "main.h"


void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


void EXTI1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

}

void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR==0x10){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

	}else if(EXTI->PR==0x20){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

	}else if(EXTI->PR==0x80){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR == 0x1000)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	}
	if(EXTI->PR == 0x4000)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
	}


}

