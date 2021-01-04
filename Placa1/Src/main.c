/*
 * main.c
 *
 *  Created on: 28 dic. 2020
 *      Author: Andrés Zapata
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"


void SystemClock_Config(void);
void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init();
void CAN1_Init();
void CAN1_Tx(uint8_t value);
void Error_handler();

CAN_RxHeaderTypeDef RxHeader;
CAN_HandleTypeDef hcan1;
uint8_t effect=0;


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	//SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	CAN1_Init();
	CAN_Filter_Config();


	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY|CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_BUSOFF)!= HAL_OK)
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

}

void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef Osc_Init;
	RCC_ClkInitTypeDef Clock_Init;
    uint8_t flash_latency=0;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE ;
	Osc_Init.HSEState = RCC_HSE_ON;
	Osc_Init.PLL.PLLState = RCC_PLL_ON;
	Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	switch(clock_freq)
	 {
	  case SYS_CLOCK_FREQ_50_MHZ:
		  Osc_Init.PLL.PLLM = 4;
		  Osc_Init.PLL.PLLN = 50;
		  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
		  Osc_Init.PLL.PLLQ = 2;
		  //Osc_Init.PLL.PLLR = 2;
		  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
		  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
          flash_latency = 1;
	     break;

	  case SYS_CLOCK_FREQ_84_MHZ:
		  Osc_Init.PLL.PLLM = 4;
		  Osc_Init.PLL.PLLN = 84;
		  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
		  Osc_Init.PLL.PLLQ = 2;
		  //Osc_Init.PLL.PLLR = 2;
		  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
		  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
          flash_latency = 2;
	     break;

	  case SYS_CLOCK_FREQ_120_MHZ:
		  Osc_Init.PLL.PLLM = 4;
		  Osc_Init.PLL.PLLN = 120;
		  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
		  Osc_Init.PLL.PLLQ = 2;
		  //Osc_Init.PLL.PLLR = 2;
		  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
		  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
          flash_latency = 3;
	     break;

	  default:
	   return ;
	 }

		if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)
	{
			Error_handler();
	}



	if (HAL_RCC_ClockConfig(&Clock_Init, flash_latency) != HAL_OK)
	{
		Error_handler();
	}


	/*Configure the systick timer interrupt frequency (for every 1 ms) */
	uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
	HAL_SYSTICK_Config(hclk_freq/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);



 }


void GPIO_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIOBtn;


	GPIOBtn.Pin = GPIO_PIN_15;
	GPIOBtn.Mode = GPIO_MODE_IT_FALLING;
	GPIOBtn.Pull = GPIO_NOPULL;
	GPIOBtn.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOB,&GPIOBtn);

	GPIOBtn.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB,&GPIOBtn);

	GPIOBtn.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB,&GPIOBtn);

	GPIOBtn.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOB,&GPIOBtn);

	GPIOBtn.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB,&GPIOBtn);

	GPIOBtn.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOB,&GPIOBtn);


	// user btn
	GPIOBtn.Pin = GPIO_PIN_13;
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


	// Enable the IRQ and set up the priority (NVIC settings )
	//HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	//HAL_NVIC_SetPriority(EXTI0_IRQn,15,0);


	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_SetPriority(EXTI4_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,15,0);


}



void CAN1_Tx(uint8_t value)
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;

	uint8_t message[8];

	TxHeader.DLC = 8;
	TxHeader.StdId = 0x5B0;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	switch(value){

		case 0:
			message[0]=0x0;
			message[1]=0x0;
			message[2]=0x1; //trama correspondiente a 2^16
			break;
		case 1:
			message[0]=0x0;
			message[1]=0x1;
			message[2]=0x1;
			break;
		case 4:
			message[0]=0x0;
			message[1]=0x2;
			message[2]=0x1;
			break;
		case 5:
			message[0]=0x0;
			message[1]=0x3;
			message[2]=0x1;
			break;
		case 6:
			message[0]=0x0;
			message[1]=0x4;
			message[2]=0x1;
			break;
		case 8:
			if(effect<4){
				message[0]=0x0;
				message[1]=0x0;
				message[2]=0x1;
				message[1] &= ~(3);
				message[1] |= (effect);
				effect++;
			}else{
				message[0]=0x0;
				message[1]=0x0;
				message[2]=0x1;
				effect=0;
			}
			break;
		case 10:			// alarma
			message[0]=0x2;
			message[1]=0x0;
			message[2]=0x1;
			break;
	}


	if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,&message,&TxMailbox) != HAL_OK)
	{
		Error_handler();
	}


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
	can1_filter_init.FilterIdHigh = 0x0000;
	can1_filter_init.FilterIdLow = 0x0000;
	can1_filter_init.FilterMaskIdHigh = 0X0000;
	can1_filter_init.FilterMaskIdLow = 0x0000;
	can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if( HAL_CAN_ConfigFilter(&hcan1,&can1_filter_init) != HAL_OK)
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

		if(RxHeader.StdId == 0x5A0 && RxHeader.RTR == 0 )
	{
		//  alarm
		if (rx_msg[0] == 0x2)
		{
			 HAL_CAN_Stop(hcan);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

		}

	}

}


void Error_handler()
{
	while(1);
}

