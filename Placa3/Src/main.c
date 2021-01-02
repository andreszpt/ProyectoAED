/*
 * main.c
 *
 *  Created on: 30 dic. 2020
 *      Author: Andrés Zapata
 */


#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"

#define MY_ADDR 0x0
#define SLAVE_ADDR  0x1

I2C_HandleTypeDef hi2c1;
uint8_t RxBuffer;



int main(void)
{
	HAL_Init();
	GPIO_Init();
	I2C1_Init();


	while(1);

	return 0;

}


void GPIO_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIOLed;

	GPIOLed.Pin = GPIO_PIN_9;
	GPIOLed.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOLed.Speed = GPIO_SPEED_MEDIUM;
	GPIOLed.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOB,&GPIOLed);

	GPIOLed.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB,&GPIOLed);

	GPIOLed.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOB,&GPIOLed);

	GPIOLed.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOB,&GPIOLed);

	GPIOLed.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB,&GPIOLed);
}

void I2C1_Init()
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = MY_ADDR;

	HAL_I2C_Init (&hi2c1);

}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	uint8_t rx_msg;

	if(HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)rx_msg, sizeof(rx_msg)) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}

	if ((rx_msg & 0x3) == 0x0) //single led
	{
		if ((rx_msg & 0x1C) == 0x0) // led 0
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_9);
			}
		}
		else if ((rx_msg & 0x1C) == 0x4) // led 1
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_10);
			}
		}
		else if ((rx_msg & 0x1C) == 0x8) // led 2
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_11);
			}
		}
		else if ((rx_msg & 0x1C) == 0xC) // led 3
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
			}
		}
		else if ((rx_msg & 0x1C) == 0x10) // led 4
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15);
			}
		}
	}
	else if ((rx_msg & 0x3) == 0x1) //effect
	{
		if ((rx_msg & 0x1C) == 0x0) // effect 0
		{

		}
		else if ((rx_msg & 0x1C) == 0x4) // effect 1
		{

		}
		else if ((rx_msg & 0x1C) == 0x8) // effect 2
		{

		}
		else if ((rx_msg & 0x1C) == 0xC) // effect 3
		{

		}
	}
	else if ((rx_msg & 0x3) == 0x2) //alarm
	{

	}



}


void Error_handler()
{
	while(1);
}
