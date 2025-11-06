/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "button.h"
#include "software_timer.h"
#include "led7_seg.h"
#include "picture.h"
#include "lcd.h"
#include <stdbool.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LEDON			1
#define LEDOFF			0
#define BEGIN			10
#define RED_GREEN		100
#define RED_YELLOW		101
#define GREEN_RED		102
#define YELLOW_RED		103

#define MOD_RED			12
#define MOD_GREEN		13
#define MOD_YELLOW		14

#define BTN_MODE		5
#define BTN_INC			6
#define BTN_SET			7


#define INIT_RED_TIME		5 - 1
#define INIT_GREEN_TIME		3 - 1
#define INIT_YELLOW_TIME	2 - 1

#define MAX_TIME		99
#define MIN_TIME		1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init(void);
void test_lcd (void);
void test_button(void);
void fsm_traffic(void);
void write_traffic_light_1(void);
void write_traffic_light_2(void);
void led7_display(int8_t light1, int8_t light2);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t g_state = BEGIN;
uint8_t g_clock_count = 0;
int8_t g_clock[4] = {0,0,0,0};
bool past_1_second = 0;

int8_t g_time_light_1 = 0;
int8_t g_time_light_2 = 0;

int8_t g_config_red_time = INIT_RED_TIME;
int8_t g_config_green_time = INIT_GREEN_TIME;
int8_t g_config_yellow_time = INIT_YELLOW_TIME;

int8_t g_temp_mod_time = 0;
uint8_t g_blink_counter = 0;
bool g_manual = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  lcd_Clear(WHITE);
  test_lcd();
  write_traffic_light_1();
  write_traffic_light_2();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  while(!flag_timer2);
	  flag_timer2 = 0;
	  g_clock_count = ( g_clock_count + 1 ) % 21;

	  // Blink Mode_Led
	  g_blink_counter = ( g_blink_counter + 1) % 20;
	  if(g_clock_count >= 20){
		  past_1_second = 1;
		  if(g_state == RED_GREEN || g_state == RED_YELLOW || g_state == GREEN_RED || g_state == YELLOW_RED){
			  g_time_light_1--;
			  g_time_light_2--;
		  }
	  }
	  button_Scan();
//	  test_button();
	  fsm_traffic();
	  if(!g_manual){
		  led7_display(g_time_light_1, g_time_light_2);
	  }else{
		  led7_display(g_temp_mod_time, g_temp_mod_time);
	  }
//	  if(button_count[0] == 1)
//		  HAL_GPIO_TogglePin( DEBUG_LED_GPIO_Port,DEBUG_LED_Pin );
//
//	  // Press 2s = 40 * 50
//	  if( button_count [1] == 40)
//		  HAL_GPIO_TogglePin( DEBUG_LED_GPIO_Port , DEBUG_LED_Pin );

	  past_1_second = 0;

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(void){
	HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, LEDOFF);
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, LEDOFF);
	HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, LEDOFF);
	g_clock[1] = 5;

	timer_init();
	led7_init();
	button_init();
	lcd_init();
	setTimer2(50);
}
void test_button(){
	for (int i = 0; i < 16; i ++) {
		if( button_count [i] == 1){
			lcd_ShowIntNum (140 , 105 , i, 2, BRED , WHITE , 32) ;
		}
	}
}
void test_lcd (){
	lcd_Fill (0, 0, 240 , 20, CYAN );
	lcd_StrCenter (0, 2, " TRAFFIC LIGHT ", BLACK , BLUE , 16,1);
	lcd_StrCenter (0 , 30, "NORMAL", WHITE , BLACK , 24,0);
}
void write_lcd(char * str, uint16_t textColor, uint16_t bgColor){
	lcd_Fill(0, 30, 240, 54, WHITE);
	lcd_StrCenter (0 , 30, str, textColor , bgColor , 24,0);
}
void write_traffic_light_1(void){
	lcd_ShowStr(10, 60, "Light 1", BLACK, WHITE, 24, 0);
	lcd_ShowStr(175, 60, "00", RED, BLACK, 24, 0);
	lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
	lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
	lcd_DrawCircle (180 , 120 , BLACK , 20, 1);
}
void write_traffic_light_2(void){
	lcd_ShowStr(10, 160, "Light 2", BLACK, WHITE, 24, 0);
	lcd_ShowStr(175, 160, "00", RED, BLACK, 24, 0);
	lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
	lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
	lcd_DrawCircle (180 , 220 , BLACK , 20, 1);
}

bool validate_traffic_time(void){
	if( g_config_red_time + 1 != g_config_green_time + g_config_yellow_time + 2){
		g_config_green_time = INIT_GREEN_TIME;
		g_config_red_time = INIT_RED_TIME;
		g_config_yellow_time = INIT_YELLOW_TIME;
		lcd_ShowStr(90, 200, "", RED, BLACK, 24, 0);
//		lcd_Fill(90, 200, 90, 30, WHITE);
		return 0;
	}
	return 1;
}

void fsm_traffic(void){
	switch(g_state){
		case BEGIN :
			if(!validate_traffic_time()){
				lcd_Fill(80, 260,160 , 284, RED);
				lcd_ShowStr(90, 260, "Error", RED, BLACK, 24, 0);
			}
			write_lcd("NORMAL", WHITE, BLACK);

			g_state = RED_GREEN;
			g_time_light_1 = g_config_red_time;
			g_time_light_2 = g_config_green_time;
			break;
		case RED_GREEN:
			if(button_count[BTN_MODE] == 1){
				g_manual = 1;
				g_temp_mod_time = g_config_red_time + 1;
				g_state = MOD_RED;
				g_blink_counter = 0;
			}
			HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, LEDON);

			// Light 1
			lcd_DrawCircle (60 , 120 , RED , 20, 1);
			lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 120 , BLACK , 20, 1);

			// Light 2
			lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 220 , GREEN , 20, 1);
			lcd_DrawCircle (180 , 220 , BLACK , 20, 1);

			if(past_1_second){
				if(g_time_light_2 < 0){
					g_state = RED_YELLOW;
					g_time_light_2 = g_config_yellow_time;
				}
				if(g_time_light_2 == g_config_green_time - 1)
				lcd_Fill(80, 260,160 , 290, WHITE);
			}
			break;
		case RED_YELLOW:
			if(button_count[BTN_MODE] == 1){
				g_manual = 1;
				g_temp_mod_time = g_config_red_time + 1;
				g_state = MOD_RED;
				g_blink_counter = 0;
			}
			if(past_1_second){
				if(g_time_light_1 < 0){
					g_state = GREEN_RED;
					g_clock[1] = 3;
					g_time_light_1 = g_config_green_time;
					g_time_light_2 = g_config_red_time;
				}
			}
			lcd_DrawCircle (60 , 120 , RED , 20, 1);
			lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 120 , BLACK , 20, 1);

			// Light 2
			lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 220 , YELLOW , 20, 1);

			break;

		case GREEN_RED:
			if(button_count[BTN_MODE] == 1){
				g_manual = 1;
				g_temp_mod_time = g_config_red_time + 1;
				g_state = MOD_RED;
				g_blink_counter = 0;
			}
			if(past_1_second){
				if(g_time_light_1 < 0){
					g_state = YELLOW_RED;
					g_clock[1] = 2;
					g_time_light_1 = g_config_yellow_time;
				}
			}
			lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 120 , GREEN , 20, 1);
			lcd_DrawCircle (180 , 120 , BLACK , 20, 1);

			// Light 2
			lcd_DrawCircle (60 , 220 , RED , 20, 1);
			lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 220 , BLACK , 20, 1);

			break;

		case YELLOW_RED:
			if(button_count[BTN_MODE] == 1){
				g_manual = 1;
				g_temp_mod_time = g_config_red_time + 1;
				g_state = MOD_RED;
				g_blink_counter = 0;
				write_lcd("MODIFICATION", WHITE, RED);
			}
			if(past_1_second){
				if(g_time_light_2 < 0){
					g_state = RED_GREEN;
					g_clock[1] = 5;
					g_time_light_1 = g_config_red_time;
					g_time_light_2 = g_config_green_time;
				}
			}
			lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 120 , YELLOW , 20, 1);

			// Light 2
			lcd_DrawCircle (60 , 220 , RED , 20, 1);
			lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 220 ,  BLACK, 20, 1);

			break;
		case MOD_RED:
			HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, LEDON);
			//Light 1
			lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 120 , BLACK , 20, 1);

			//Light 2
			lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 220 ,  BLACK, 20, 1);

			if(g_blink_counter < 10){
				lcd_DrawCircle (60 , 120 , RED , 20, 1);
				lcd_DrawCircle (60 , 220 , RED , 20, 1);
			} else {
				lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
				lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
			}
			// BTN_INC
			if(button_count[BTN_INC] == 1){
				g_temp_mod_time++;
				if(g_temp_mod_time > MAX_TIME) g_temp_mod_time = MIN_TIME;
			}
			// BTN_SET
			if(button_count[BTN_SET] == 1){
				g_config_red_time = g_temp_mod_time - 1;
			}
			// BTN_MODE
			if(button_count[BTN_MODE] == 1){
				g_state = MOD_GREEN;
				g_temp_mod_time = g_config_green_time + 1;
				g_blink_counter = 0;
				write_lcd("MODIFICATION", WHITE, GREEN);
			}

			break;
		case MOD_GREEN:
			//Light 1
			lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 120 , BLACK , 20, 1);

			//Light 2
			lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (180 , 220 ,  BLACK, 20, 1);

			if(g_blink_counter < 10){
				lcd_DrawCircle (120 , 120 , GREEN , 20, 1);
				lcd_DrawCircle (120 , 220 , GREEN , 20, 1);
			} else {
				lcd_DrawCircle (120 , 120 , BLACK , 20, 1);
				lcd_DrawCircle (120 , 220 , BLACK , 20, 1);
			}
			// BTN_INC
			if(button_count[BTN_INC] == 1){
				g_temp_mod_time++;
				if(g_temp_mod_time > MAX_TIME) g_temp_mod_time = MIN_TIME;
			}
			// BTN_SET
			if(button_count[BTN_SET] == 1){
				g_config_green_time = g_temp_mod_time - 1; // Lưu giá trị (N-1)
			}
			// BTN_MODE
			if(button_count[BTN_MODE] == 1){
				g_state = MOD_YELLOW;
				g_temp_mod_time = g_config_yellow_time + 1;
				g_blink_counter = 0;
				write_lcd("MODIFICATION", WHITE, YELLOW);
			}

			break;

		case MOD_YELLOW:
			if(button_count[BTN_MODE] == 1){
				g_state = GREEN_RED;
			}

			//Light 1
			lcd_DrawCircle (60 , 120 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 120 , BLACK , 20, 1);

			//Light 2
			lcd_DrawCircle (60 , 220 , BLACK , 20, 1);
			lcd_DrawCircle (120 , 220 , BLACK , 20, 1);

			if(g_blink_counter < 10){
				lcd_DrawCircle (180 , 120 , YELLOW , 20, 1);
				lcd_DrawCircle (180 , 220 , YELLOW , 20, 1);
			} else {
				lcd_DrawCircle (180 , 120 , BLACK , 20, 1);
				lcd_DrawCircle (180 , 220 , BLACK , 20, 1);
			}

			// BTN_INC
			if(button_count[BTN_INC] == 1){
				g_temp_mod_time++;
				if(g_temp_mod_time > MAX_TIME) g_temp_mod_time = MIN_TIME;
			}
			// BTN_SET
			if(button_count[BTN_SET] == 1){
				g_config_yellow_time = g_temp_mod_time - 1;
			}
			// BTN_MODE
			if(button_count[BTN_MODE] == 1){
				g_manual = 0;
				g_state = BEGIN;
				HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, LEDOFF);
				g_blink_counter = 0;
			}
			break;


		default:
			break;
	}
}


void led7_display(int8_t light1, int8_t light2){

	char str_light_1[5];
	char str_light_2[5];

	snprintf(str_light_1, 5, "%02d", light1);
	snprintf(str_light_2, 5, "%02d", light2);

	lcd_ShowStr(175, 60, str_light_1, RED, BLACK, 24, 0);
	lcd_ShowStr(175, 160, str_light_2, RED, BLACK, 24, 0);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
