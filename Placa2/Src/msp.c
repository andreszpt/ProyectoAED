/*
 * msp.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andr�s Zapata
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

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_CAN1_CLK_ENABLE();

	/**CAN1 GPIO Configuration
	PA11     ------> CAN1_RX
	PA12     ------> CAN1_TX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(CAN1_TX_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn,15,0);

	HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

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
