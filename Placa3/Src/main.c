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

void SystemClock_Config(void);
void GPIO_Init();
void I2C1_Init();
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void Error_handler(void);


I2C_HandleTypeDef hi2c1;
uint8_t RxBuffer;
TIM_HandleTypeDef htimer6;


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	GPIO_Init();
	I2C1_Init();
	TIMER6_Init();

	HAL_TIM_Base_Start_IT(&htimer6);


	while(1);

	return 0;

}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /**Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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


	//board leds (alarms)
	GPIOLed.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB,&GPIOLed);
	GPIOLed.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB,&GPIOLed);
	GPIOLed.Pin = GPIO_PIN_14;
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

void TIMER6_Init(void)
{
	/*
	 * Timer_clk = 84MHz
	 * Prescaler = 299
	 * Cnt_clk = 280kHz
	 * t_period = 3.571e-6
	 * Period = 50e-3/t_period = 14000
	 */
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 299;
	htimer6.Init.Period = 14000;
	if ( HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//transmit alarm (I2C)
}


void Error_handler(void)
{
	while(1);
}
