
#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htimer6;
extern void CAN1_Tx(uint8_t remote);

void EXTI0_IRQHandler(void)
{
	CAN1_Tx(0);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

}

void EXTI1_IRQHandler(void)
{
	CAN1_Tx(1);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

}

void EXTI2_IRQHandler(void)
{
	CAN1_Tx(4);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR==0x10){
		CAN1_Tx(5);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

	}else if(EXTI->PR==0x20){
		CAN1_Tx(6);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

	}else if(EXTI->PR==0x80){
		CAN1_Tx(8);
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

	}
}

