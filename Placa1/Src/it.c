/*
 * it.c
 *
 *  Created on: 28 dic. 2020
 *      Author: Andr�s Zapata
 */

#include "main.h"

extern void CAN1_Tx(uint8_t remote);

void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


void EXTI1_IRQHandler(void)
{
	CAN1_Tx(1);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

}

void EXTI2_IRQHandler(void)
{
	CAN1_Tx(4);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR==0x10){
		CAN1_Tx(5);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

	}else if(EXTI->PR==0x20){
		CAN1_Tx(6);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

	}else if(EXTI->PR==0x80){
		CAN1_Tx(8);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR == 0x1000)
	{
		CAN1_Tx(10);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	}
	if(EXTI->PR == 0x4000)
	{
		CAN1_Tx(0);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
	}


}

