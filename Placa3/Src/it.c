/*
 * it.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andr�s Zapata
 */


void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

