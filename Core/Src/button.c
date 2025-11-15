/*
 * button.c
 *
 *  Created on: Nov 3, 2025
 *      Author: lesih
 */

#ifndef SRC_BUTTON_C_
#define SRC_BUTTON_C_

#include "button.h"

uint16_t button_count[16];
uint16_t spi_button = 0x0000;
SPI_HandleTypeDef hspi1;
void button_init(){
	HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 1);
}

void button_Scan(){
	  HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 0);
	  HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 1);
	  HAL_SPI_Receive(&hspi1, (void*)&spi_button, 2, 10);
	  int button_index = 0;
	  uint16_t mask = 0x8000;
	  for(int i = 0; i < 16; i++){
		  if(i >= 0 && i <= 3){
			  button_index = i + 4;
		  } else if (i >= 4 && i <= 7){
			  button_index = 7 - i;
		  } else if (i >= 8 && i <= 11){
			  button_index = i + 4;
		  } else {
			  button_index = 23 - i;
		  }
		  if(spi_button & mask) button_count[button_index] = 0;
		  else button_count[button_index]++;

		  mask = mask >> 1;
	  }
}

#endif /* SRC_BUTTON_C_ */
