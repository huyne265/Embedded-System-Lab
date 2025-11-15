/*
 * uart.h
 *
 * Created on: Nov 13, 2025
 * Author: lesih (Modified by Gemini)
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"
#include <stdio.h>
#include <string.h>

#define UART_BUFFER_SIZE 128

extern UART_HandleTypeDef huart1;

extern volatile uint8_t uart_rx_flag;


// --- Khai báo Hàm ---
void uart_init_rs232(void);

void uart_Rs232SendString(uint8_t* str);

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size);

void uart_Rs232SendNum(uint32_t num);

void uart_Rs232SendNumPercent(uint32_t num);


uint16_t uart_data_available(void);

int16_t uart_read_byte(void);


#endif /* INC_UART_H_ */
