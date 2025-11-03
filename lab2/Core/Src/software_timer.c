/*
 * software_timer.c
 *
 *  Created on: Oct 8, 2025
 *      Author: a5159165
 */

#include "software_timer.h"
#include "global.h"

//software timer variable
uint16_t flag_timer2 = 0;
uint16_t timer2_counter = 0;
uint16_t timer2_MUL = 0;
uint16_t count_led_7seg = 0;

/**
  * @brief  Set duration of software timer interrupt
  * @param  duration Duration of software timer interrupt
  * @retval None
  */
void setTimer2(uint16_t duration){
	timer2_MUL = duration/TIMER_CYCLE_2;
	timer2_counter = timer2_MUL;
	flag_timer2 = 0;
}

/**
  * @brief  Timer interrupt routine
  * @param  htim TIM Base handle
  * @note	This callback function is called by system
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		if(timer2_counter > 0){
			timer2_counter--;
			if(timer2_counter == 0) {
				flag_timer2 = 1;
				timer2_counter = timer2_MUL;
			}
		}
		// 1ms interrupt here
		count_led_7seg = (count_led_7seg + 1 ) % (CYCLE/ 4);
		if(count_led_7seg == 0 ){
			led7_Scan();
		}
	}
}

