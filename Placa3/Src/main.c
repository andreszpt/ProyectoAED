
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"

#define MY_ADDR 0x1
#define MASTER_ADDR  0x0

void SystemClock_Config(void);
void GPIO_Init();
void I2C1_Init();
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void TIMER6_Init(void);
void TIMER7_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Error_handler(void);


I2C_HandleTypeDef hi2c1;
uint8_t RxBuffer;
TIM_HandleTypeDef htimer6;
TIM_HandleTypeDef htimer7;

uint8_t efecto;
uint8_t efecto_on;
uint8_t cont=0;


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	GPIO_Init();
	I2C1_Init();
	TIMER6_Init();
	TIMER7_Init();

	HAL_TIM_Base_Start_IT(&htimer6);
	HAL_TIM_Base_Start_IT(&htimer7);

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
    Error_handler();
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
    Error_handler();
  }
}


void GPIO_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

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


	// user btn
	GPIO_InitTypeDef GPIOBtn;

	GPIOBtn.Pin = GPIO_PIN_13;
	GPIOBtn.Mode = GPIO_MODE_IT_FALLING;
	GPIOBtn.Pull = GPIO_NOPULL;
	GPIOBtn.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOC,&GPIOBtn);
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
		Error_handler();
	}

	if ((rx_msg & 0x3) == 0x0) //single led
	{
		efecto_on=0;
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
		efecto_on=1;
		if ((rx_msg & 0x1C) == 0x0) // effect 0
		{
			efecto=0;
		}
		else if ((rx_msg & 0x1C) == 0x4) // effect 1
		{
			efecto=1;
		}
		else if ((rx_msg & 0x1C) == 0x8) // effect 2
		{
			efecto=2;
		}
		else if ((rx_msg & 0x1C) == 0xC) // effect 3
		{
			efecto=3;
		}
	}
	else if ((rx_msg & 0x3) == 0x2) //alarm
	{
		efecto_on=0;
		HAL_TIM_Base_Stop_IT(&htimer6);
		HAL_TIM_Base_Stop_IT(&htimer7);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
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

void TIMER7_Init(void)
{
	/*
	 * Timer_clk = 84MHz
	 * Prescaler = 2999
	 * Cnt_clk = 280kHz
	 * t_period = 3.571e-6
	 * Period = 50e-3/t_period = 28000
	 */
	htimer7.Instance = TIM7;
	htimer7.Init.Prescaler = 2999;
	htimer7.Init.Period = 28000;
	if ( HAL_TIM_Base_Init(&htimer7) != HAL_OK)
	{
		Error_handler();
	}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		uint8_t tx_msg;

		//transmit alarm (I2C)
		if( HAL_GPIO_ReadPin( GPIOC, GPIO_PIN_13))
		{
			tx_msg=1;
			if(HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t *)tx_msg, sizeof(tx_msg)) != HAL_OK)
			{
				/* Transfer error in transmission process */
				Error_handler();
			}
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		}
		else
		{
			tx_msg=0;
		}
	}

	else if(htim->Instance == TIM7)
	{
		if(efecto_on==1)
		{
			switch (efecto)
			{

			case 0:		//efecto 0
				switch(cont)
				{
				case 0:		// iteracion 0
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 3:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 4:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_SET);
					cont=0;
					break;
				}
				efecto++;
				break;

			case 1:
				switch(cont)
				{
				case 0:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_SET);
					cont++;
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont=0;
					break;

				}
				efecto++;
				break;


			case 2:
				switch(cont)
				{
				case 0:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_SET);
					cont++;
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont=0;
					break;

				}
				efecto++;
				break;

			case 3:
				switch(cont)
				{
				case 0:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_SET);
					cont++;
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont=0;
					break;
				}
				efecto=0;
				break;
			}
		}
	}
}





void Error_handler(void)
{
	while(1);
}

