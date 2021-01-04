/*
 * it.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andrés Zapata
 */

extern TIM_HandleTypeDef htimer6;


void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);

}
