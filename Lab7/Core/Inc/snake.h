/*
 * snake.h
 *
 *  Created on: Nov 17, 2025
 *      Author: a5159165
 */
#include "stdbool.h"
#include "stdio.h"
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"

#ifndef INC_SNAKE_H_
#define INC_SNAKE_H_

#define MAX_SNAKE_LEN 100
#define GRID_SIZE 10
#define GRID_WIDTH 24
#define GRID_HEIGHT 22

#define SNAKE_SPEED 20 	// 4 * 50ms

#define SNAKE_HEAD_COLOR CYAN
#define SNAKE_BODY_COLOR WHITE
#define SNAKE_BORDER_COLOR BLUE

typedef struct {
    int x;
    int y;
} Point;

Point snake[MAX_SNAKE_LEN];
char score_str[20];
int snake_length;
Point food;
int direction; // 0: Up, 1: Down, 2: Left, 3: Right
int score;
int is_game_over;
int game_tick_counter;


void draw_GridSquare(int x, int y, uint16_t color);
void spawn_Food(void);
void game_Init(void);
void update_Game(void);
void update_Score_Display(void);


#endif /* INC_SNAKE_H_ */
