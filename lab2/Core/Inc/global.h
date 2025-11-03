/*
 * global.h
 *
 *  Created on: Oct 8, 2025
 *      Author: a5159165
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>
#include "main.h"
#include "software_timer.h"
#include "led_7seg.h"

#define LED_ON		1
#define LED_OFF		0
#define HZ_1		1
#define HZ_25		25
#define HZ_100		100
#define CYCLE		1000/HZ_100
#define COUNT_TICK	(CYCLE * 1000 / 2)

extern int clock_time[4];

#endif /* INC_GLOBAL_H_ */
