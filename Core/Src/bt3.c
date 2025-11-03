/*
 * bt3.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lesih
 */

#include "bt3.h"
int bt3_cnt = 0;
void bt3_init(){
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, 0);
	HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, 0);
}

void bt3(){
	if(bt3_cnt < 5){
		HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, 1);
		HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, 0);
		++bt3_cnt;
	}else if(bt3_cnt < 8){
		HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 1);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, 0);
		HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, 0);
		++bt3_cnt;
	}else if(bt3_cnt < 9){
		HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, 0);
		HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, 1);
		++bt3_cnt;
	}else{
		bt3_cnt = 0;
	}
	HAL_Delay(1000);
}
