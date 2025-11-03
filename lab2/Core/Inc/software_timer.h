/*
 * software_timer.h
 *
 *  Created on: Oct 8, 2025
 *      Author: a5159165
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>
#include "main.h"

#define TIMER_CYCLE_2 1


//software timer variable
extern uint16_t flag_timer2;
uint16_t timer2_counter;
uint16_t timer2_MUL;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void setTimer2(uint16_t duration);


#endif /* INC_SOFTWARE_TIMER_H_ */
