/*
 * software_timer.c
 *
 *  Created on: Nov 3, 2025
 *      Author: lesih
 */
#include "software_timer.h"

uint8_t flag_timer1 = 0;
uint16_t timer1_counter = 0;
uint16_t timer1_MUL = 0;

void setTimer1(uint16_t duration){
	timer1_MUL = duration;
	timer1_counter = timer1_MUL;
	flag_timer1 = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		if(timer1_counter > 0){
			timer1_counter--;
			if(timer1_counter == 0) {
				flag_timer1 = 1;
				timer1_counter = timer1_MUL;
			}
		}
	}
}
