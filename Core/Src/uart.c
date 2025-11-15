/*
 * uart.c
 *
 * Created on: Nov 13, 2025
 * Author: lesih (Modified by Gemini)
 */

#include "uart.h"
#include <stdio.h>
#include <string.h>

static uint8_t rx_buffer[UART_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;
static uint8_t receive_buffer1 = 0;

volatile uint8_t uart_rx_flag = 0;

static uint8_t msg[100];


void uart_init_rs232(void) {
    // Reset buffer
    rx_head = 0;
    rx_tail = 0;
    uart_rx_flag = 0;

    HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        uint16_t next_head = (rx_head + 1) % UART_BUFFER_SIZE;

        if (next_head != rx_tail) {
            rx_buffer[rx_head] = receive_buffer1;
            rx_head = next_head;
            uart_rx_flag = 1;
        }

        HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
    }
}

uint16_t uart_data_available(void) {
    return (rx_head - rx_tail + UART_BUFFER_SIZE) % UART_BUFFER_SIZE;
}


int16_t uart_read_byte(void) {
    if (rx_tail == rx_head) {
        return -1;
    }

    uint8_t byte = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_BUFFER_SIZE;

    if (rx_tail == rx_head) {
        uart_rx_flag = 0;
    }

    return byte;
}

void uart_Rs232SendString(uint8_t* str) {
    uint16_t len = sprintf((void*)msg, "%s", str);
    HAL_UART_Transmit(&huart1, msg, len, 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size) {
    HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num) {
    uint16_t len = sprintf((void*)msg, "%lu", (unsigned long)num);
    HAL_UART_Transmit(&huart1, msg, len, 10);
}

void uart_Rs232SendNumPercent(uint32_t num) {
    uint16_t len = sprintf((void*)msg, "%lu.%02lu", (unsigned long)num / 100, (unsigned long)num % 100);
    HAL_UART_Transmit(&huart1, msg, len, 10);
}
