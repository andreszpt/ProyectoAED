/*
 * msp.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andrés Zapata
 */

#include "main.h"


void HAL_MspInit(void)
{
 //Here will do low level processor specific inits.
	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= 0x7 << 16; //usage fault, memory fault and bus fault system exceptions

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

	  __HAL_RCC_I2C1_CLK_ENABLE();

	  /* I2C1 GPIO Config
	   * PB6-> SCL
	   * PB7 -> SDA
	   */

		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(I2C1_EV_IRQn,15,0);
		HAL_NVIC_SetPriority(I2C1_ER_IRQn,15,0);


		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{

	//1. enable the clock for the TIM6 peripheral
	__HAL_RCC_TIM6_CLK_ENABLE();

	//2. Enable the IRQ of TIM6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//3. setup the priority for TIM6_DAC_IRQn
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,15,0);

	//1. enable the clock for the TIM7 peripheral
	__HAL_RCC_TIM7_CLK_ENABLE();

	//2. Enable the IRQ of TIM7
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	//3. setup the priority for TIM7_IRQn
	HAL_NVIC_SetPriority(TIM7_IRQn,15,0);

}
