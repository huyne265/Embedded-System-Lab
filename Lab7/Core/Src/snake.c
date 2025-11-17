/*
 * snake.c
 *
 *  Created on: Nov 17, 2025
 *      Author: a5159165
 */
#include "snake.h"

int game_tick_counter = 0;
void draw_GridSquare(int x, int y, uint16_t color) {
    lcd_Fill(x * GRID_SIZE, y * GRID_SIZE,
             (x + 1) * GRID_SIZE - 1, (y + 1) * GRID_SIZE - 1,
             color);
}


void spawn_Food() {
    while(1) {
        food.x = 1 + rand() % (GRID_WIDTH - 2);
        food.y = 1 + rand() % (GRID_HEIGHT - 2);

        int on_snake = 0;
        for (int i = 0; i < snake_length; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                on_snake = 1;
                break;
            }
        }
        if (!on_snake) break;
    }
    draw_GridSquare(food.x, food.y, GREEN);
}


void game_Init() {
    lcd_Fill(0, 0, 240, 220, BLACK);

    uint16_t wall_color = BLUE;
    for (int x = 0; x < GRID_WIDTH; x++) {
		draw_GridSquare(x, 0, wall_color);
		draw_GridSquare(x, GRID_HEIGHT - 1, wall_color);
	}
    for (int y = 1; y < GRID_HEIGHT - 1; y++) {
		draw_GridSquare(0, y, wall_color);
		draw_GridSquare(GRID_WIDTH - 1, y, wall_color);
	}
    lcd_Fill(0, 220, 239, 221, WHITE);

    snake_length = 3;
    snake[0].x = GRID_WIDTH / 2;
    snake[0].y = GRID_HEIGHT / 2;
    snake[1].x = GRID_WIDTH / 2 - 1;
    snake[1].y = GRID_HEIGHT / 2;
    snake[2].x = GRID_WIDTH / 2 - 2;
    snake[2].y = GRID_HEIGHT / 2;

    direction = 3;
    score = 0;
    is_game_over = 0;
    game_tick_counter = 0;

    for (int i = 0; i < snake_length; i++) {
        draw_GridSquare(snake[i].x, snake[i].y, WHITE);
    }

    spawn_Food();
    update_Score_Display();

    draw_FancySegment(snake[0].x, snake[0].y, SNAKE_HEAD_COLOR, SNAKE_BORDER_COLOR);
	for (int i = 1; i < snake_length; i++) {
		draw_FancySegment(snake[i].x, snake[i].y, SNAKE_BODY_COLOR, SNAKE_BORDER_COLOR);
	}
}


void update_Game() {
    if (is_game_over) return;

    Point new_head = snake[0];
    if (direction == 0) new_head.y--;
    if (direction == 1) new_head.y++;
    if (direction == 2) new_head.x--;
    if (direction == 3) new_head.x++;

    if (new_head.x <= 0 || new_head.x >= GRID_WIDTH - 1 ||
        new_head.y <= 0 || new_head.y >= GRID_HEIGHT - 1) {
        is_game_over = 1;
        return;
    }

    for (int i = 1; i < snake_length; i++) {
        if (snake[i].x == new_head.x && snake[i].y == new_head.y) {
            is_game_over = 1;
            return;
        }
    }


    int ate_food = 0;
    if (new_head.x == food.x && new_head.y == food.y) {
        ate_food = 1;
        score++;
        snake_length++;
        update_Score_Display();
        if (snake_length > MAX_SNAKE_LEN) snake_length = MAX_SNAKE_LEN;
    }

    if (!ate_food) {
        draw_GridSquare(snake[snake_length - 1].x, snake[snake_length - 1].y, BLACK);
    }

    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    snake[0] = new_head;
//    draw_GridSquare(snake[0].x, snake[0].y, WHITE);
    draw_FancySegment(snake[0].x, snake[0].y, SNAKE_HEAD_COLOR, SNAKE_BORDER_COLOR);
    if (snake_length > 1) {
		 draw_FancySegment(snake[1].x, snake[1].y, SNAKE_BODY_COLOR, SNAKE_BORDER_COLOR);
	}
    if (ate_food) {
        spawn_Food();
    }
}

void update_Score_Display() {
	sprintf(score_str, "SCORE: %d", score);

	lcd_Fill(10, 225, 110, 241, BLACK);
	lcd_ShowStr(10, 225, (uint8_t*)score_str, GREEN, BLACK, 16, 0);
}

void draw_FancySegment(int x, int y, uint16_t inner_color, uint16_t border_color) {
	int px = x * GRID_SIZE;
	int py = y * GRID_SIZE;

	lcd_Fill(px, py, px + GRID_SIZE - 1, py + GRID_SIZE - 1, border_color);

	lcd_Fill(px + 1, py + 1, px + GRID_SIZE - 2, py + GRID_SIZE - 2, inner_color);
}
