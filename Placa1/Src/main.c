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


void GPIO_Init();
void CAN1_Init();
void CAN1_Tx(uint8_t value);
void Error_handler();

CAN_HandleTypeDef hcan1;

int main(void)
{
	HAL_Init();


	GPIO_Init();

	CAN1_Init();


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


void GPIO_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIOBtn, GPIOLed;

	GPIOBtn.Pin = GPIO_PIN_0;
	GPIOBtn.Mode = GPIO_MODE_IT_FALLING;
	GPIOBtn.Speed = GPIO_SPEED_MEDIUM;
	GPIOBtn.Pull = GPIO_PULLUP;

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

	// Enable the IRQ and set up the priority (NVIC settings )
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_SetPriority(EXTI4_IRQn,15,0);

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,15,0);



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

uint8_t effect=0;

void CAN1_Tx(uint8_t value)
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;

	uint8_t message[3];

	TxHeader.DLC = 3;
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



void Error_handler()
{
	while(1);
}

