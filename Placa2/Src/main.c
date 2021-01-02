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



int main(void)
{
	HAL_Init();
	CAN1_Init();
	I2C1_Init();

	CAN_Filter_Config();

	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING |CAN_IT_BUSOFF)!= HAL_OK)
	{
		Error_handler();
	}

	if( HAL_CAN_Start(&hcan1) != HAL_OK)
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

	HAL_I2C_Init (&hi2c1);


}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t rx_msg[3];
	uint8_t data[3];


	if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,rx_msg) != HAL_OK)
	{
		Error_handler();
	}

	if(RxHeader.StdId == 0x5B0 && RxHeader.RTR == 0 )
	{
		// Recibo info de placa 1, transmito a placa 2
		data = rx_msg;

		if(HAL_I2C_Master_Transmit_IT(&hi2c1, SLAVE_ADDR, &data, sizeof(data)) != HAL_OK)
		{
			Error_handler();
		}
	}

}


void Error_handler()
{
	while(1);
}
