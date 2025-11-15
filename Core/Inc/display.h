/*
 * display.h
 *
 *  Created on: Nov 3, 2025
 *      Author: lesih
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "button.h"
#include "lcd.h"
#include "ds3231.h"
#include "uart.h"
#include "stdlib.h"


void initDisplay(int h,int mi, int s, int d,int dd,int m, int y);
void blinkyDisplay(uint8_t val,uint8_t pos);
void displayTime();
void displayConfigTime();
void displayAlarmTime();
void updateTime();
void configMode();
void checkAlarm();
void setAlarm();
void Display();
#endif /* INC_DISPLAY_H_ */
