/*
 * bt2.c
 *
 *  Created on: Oct 6, 2025
 *      Author: lesih
 */

#include "bt2.h"

int bt2_cnt = 0;
void bt2_init(){
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
}
void bt2(){
	if(bt2_cnt == 2){
		HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 0);
		++bt2_cnt;
	}else if(bt2_cnt == 6){
		HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, 1);
		bt2_cnt = 0;
	}else{
		++bt2_cnt;
	}
	HAL_Delay(1000);
}
