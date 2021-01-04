/*
 * it.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andr�s Zapata
 */

#include "main.h"

void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI15_10_IRQHandler(void)
{
	CAN1_Tx();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

}
