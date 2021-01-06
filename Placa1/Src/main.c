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

// Variables para controlar los efectos
uint8_t efecto;
uint8_t efecto_on;
uint8_t cont=0;


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	// Inicializaciones de los periféricos GPIO, I2C, TIM6, TIM7
	GPIO_Init();
	I2C1_Init();
	TIMER6_Init();
	TIMER7_Init();

	// Activamos los timers
	HAL_TIM_Base_Start_IT(&htimer6);
	HAL_TIM_Base_Start_IT(&htimer7);

	while(1);

	return 0;

}

// Configuración del reloj
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
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIOLed;

	// External LEDs
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


	// Board LEDs (alarms)
	GPIOLed.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB,&GPIOLed);
	GPIOLed.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB,&GPIOLed);
	GPIOLed.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB,&GPIOLed);


	// User button
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
	hi2c1.Init.OwnAddress1 = 0x1;
	hi2c1.Init.ClockSpeed = 400000;

	HAL_I2C_Init (&hi2c1);

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
	htimer6.Init.Period = 14000-1;
	if ( HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void TIMER7_Init(void)
{
	htimer7.Instance = TIM7;
	htimer7.Init.Prescaler = 2999;
	htimer7.Init.Period = 28000-1;

	if ( HAL_TIM_Base_Init(&htimer7) != HAL_OK)
	{
		Error_handler();
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	uint8_t rx_msg;		// "XX XX XX XX"

	if(HAL_I2C_Slave_Receive_IT(&hi2c1, &rx_msg, sizeof(rx_msg)) != HAL_OK)
	{
		/* Transfer error in reception process */
		Error_handler();
	}

	if ((rx_msg & 0x3) == 0x0) // Single led
	{
		efecto_on=0;
		if ((rx_msg & 0x1C) == 0x0) // LED 0
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_9);
			}
		}

		else if ((rx_msg & 0x1C) == 0x4) // LED 1
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_10);
			}
		}
		else if ((rx_msg & 0x1C) == 0x8) // LED 2
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_11);
			}
		}
		else if ((rx_msg & 0x1C) == 0xC) // LED 3
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
			}
		}
		else if ((rx_msg & 0x1C) == 0x10) // LED 4
		{
			if ((rx_msg & 0x20) == 0x1)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15);
			}
		}
	}
	else if ((rx_msg & 0x3) == 0x1) // Effect
	{
		efecto_on=1;
		if ((rx_msg & 0x1C) == 0x0) // Effect 0
		{
			efecto=0;
		}
		else if ((rx_msg & 0x1C) == 0x4) // Effect 1
		{
			efecto=1;
		}
		else if ((rx_msg & 0x1C) == 0x8) // Effect 2
		{
			efecto=2;
		}
		else if ((rx_msg & 0x1C) == 0xC) // Effect 3
		{
			efecto=3;
		}
	}
	else if ((rx_msg & 0x3) == 0x2) // Alarm
	{
		efecto_on=0;
		// Paramos los timers
		HAL_TIM_Base_Stop_IT(&htimer6);
		HAL_TIM_Base_Stop_IT(&htimer7);

		// Encendemos LEDs de la placa
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		uint8_t tx_msg;

		// Transmit alarm (I2C)
		if( HAL_GPIO_ReadPin( GPIOC, GPIO_PIN_13))
		{
			tx_msg=1;
			if(HAL_I2C_Slave_Transmit_IT(&hi2c1, &tx_msg, sizeof(tx_msg)) != HAL_OK)
			{
				/* Transfer error in transmission process */
				Error_handler();
			}

			// Encendemos LEDs de la placa
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
		if(efecto_on==1) // Efecto activado
		{
			switch (efecto)
			{

			case 0:		// Efecto 0
				switch(cont)
				{
				case 0:		// Iteración 0
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_RESET);
					cont++;
					break;
				case 1:		// Iteración 1
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

			case 1:			// Efecto 1
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


			case 2:			// Efecto 2
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

			case 3:			// Efecto 3
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

