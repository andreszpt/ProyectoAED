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
CAN_HandleTypeDef hcan1;
CAN_RxHeaderTypeDef RxHeader;

void CAN1_Init();
void CAN_Filter_Config(void);
void I2C1_Init();
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void Error_handler();
void SystemClock_Config(void);

int main(void)
{
	HAL_Init();

	SystemClock_Config();

	CAN1_Init();
	I2C1_Init();
	GPIO_Init();

	CAN_Filter_Config();

	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING |CAN_IT_BUSOFF)!= HAL_OK)
	{
		Error_handler();
	}

	if( HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_handler();
	}

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
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIOBtn;


	GPIOBtn.Pin = GPIO_PIN_13;
	GPIOBtn.Mode = GPIO_MODE_IT_FALLING;
	GPIOBtn.Pull = GPIO_NOPULL;
	GPIOBtn.Speed = GPIO_SPEED_MEDIUM;

	// user btn
	HAL_GPIO_Init(GPIOC,&GPIOBtn);

	//leds alarm

		GPIO_InitTypeDef GPIOLed;

		GPIOLed.Pin = GPIO_PIN_0;
		GPIOLed.Mode = GPIO_MODE_OUTPUT_PP;
		GPIOLed.Speed = GPIO_SPEED_MEDIUM;
		GPIOLed.Pull = GPIO_NOPULL;

		HAL_GPIO_Init(GPIOB,&GPIOLed);

		GPIOLed.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB,&GPIOLed);
		GPIOLed.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB,&GPIOLed);

}

void CAN1_Init()
{
	hcan1.Instance = CAN1;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	//Settings related to CAN bit timings
	hcan1.Init.Prescaler = 3;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

	if ( HAL_CAN_Init (&hcan1) != HAL_OK)
	{
		Error_handler();
	}

}

void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter_init;

	can1_filter_init.FilterActivation = ENABLE;
	can1_filter_init.FilterBank  = 0;
	can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	can1_filter_init.FilterIdHigh = 0x5B0;
	//can1_filter_init.FilterIdLow = 0x5B0;
	can1_filter_init.FilterMode = CAN_FILTERMODE_IDLIST;
	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if( HAL_CAN_ConfigFilter(&hcan1,&can1_filter_init) != HAL_OK)
	{
		Error_handler();
	}

}

void I2C1_Init()
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = MY_ADDR;
	hi2c1.Init.ClockSpeed = 400000;

	HAL_I2C_Init (&hi2c1);


}

void CAN1_Tx()
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;

	uint8_t message[8];

	TxHeader.DLC = 8;
	TxHeader.StdId = 0x5A0;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

			// alarma
			message[0]=0x2;
			message[1]=0x0;
			message[2]=0x1;



	if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,&message,&TxMailbox) != HAL_OK)
	{
		Error_handler();
	}


}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t rx_msg[8];
	uint8_t tx_data = 0;


	if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,rx_msg) != HAL_OK)
	{
		Error_handler();
	}

	if(RxHeader.StdId == 0x5B0 && RxHeader.RTR == 0 )
	{
		// Recibo info de placa 1, transmito a placa 2

		// single led
		if (rx_msg[0] == 0x0)
		{
			if (rx_msg[1] == 0x0)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x20; // 00100000
				}
			}
			else if (rx_msg[1] == 0x1)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x24; // 00100100
				}
			}
			else if (rx_msg[1] == 0x2)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x28; // 00101000
				}
			}
			else if (rx_msg[1] == 0x3)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x2C; // 00101100
				}
			}
			else if (rx_msg[1] == 0x4)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x30; // 00110000
				}
			}
		}

		// effect
		else if (rx_msg[0] == 0x1)
		{
			if (rx_msg[1] == 0x0)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x21; // 00100001
				}
			}
			else if (rx_msg[1] == 0x1)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x25; // 00100101
				}
			}
			else if (rx_msg[1] == 0x2)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x29; // 00101001
				}
			}
			else if (rx_msg[1] == 0x3)
			{
				if (rx_msg[2] == 0x1)
				{
					tx_data = 0x2D; // 00101101
				}
			}
		}

		//  alarm
		else if (rx_msg[0] == 0x2)
		{
			tx_data = 0x23; // 00100011 (Alarma)
		}


		if(HAL_I2C_Master_Transmit_IT(&hi2c1, SLAVE_ADDR, &tx_data, sizeof(tx_data)) != HAL_OK)
		{
			Error_handler();
		}
	}

}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	uint8_t rx_msg;

		if(HAL_I2C_Master_Receive_IT(&hi2c1, (uint8_t *)rx_msg, sizeof(rx_msg)) != HAL_OK)
		{
			/* Transfer error in transmission process */
			Error_handler();
		}

		 if ((rx_msg & 0x1) == 0x1) //alarm
		{
			  HAL_CAN_Stop(hcan1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			  CAN1_Tx();
		}

}



void Error_handler()
{
	while(1);
}
