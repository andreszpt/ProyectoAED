/*
 * it.h
 *
 *  Created on: 30 dic. 2020
 *      Author: Andrés Zapata
 */

#ifndef IT_H_
#define IT_H_

#include "main.h"

extern TIM_HandleTypeDef htimer6;

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);

}

#endif /* IT_H_ */
