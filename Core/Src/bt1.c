/*
 * bt1.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lesih
 */

#include "bt1.h"

void bt1_init(){
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
}
void bt1(){
	HAL_GPIO_TogglePin(LED_3_GPIO_Port, LED_3_Pin);
	HAL_Delay(2000);
	HAL_GPIO_TogglePin(LED_3_GPIO_Port, LED_3_Pin);
	HAL_Delay(4000);
}
