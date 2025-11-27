/*
 * display.c
 *
 *  Created on: Nov 17, 2025
 *      Author: lesih
 */
/*
 * display.c
 *
 * Created on: Nov 17, 2025
 * Author: lesih
 */
#include "display.h"
#include "lcd.h"
#include "sensor.h"
#include "ds3231.h"
#include "buzzer.h"
#include "uart.h"
#include "led_7seg.h"

#define HUMIDITY_THRESHOLD 70.0f
#define ADC_MAX_VALUE 4095.0f
#define LIGHT_THRESHOLD 2048

#define GRAPH_START_X 10
#define GRAPH_START_Y 180
#define GRAPH_WIDTH 220
#define GRAPH_HEIGHT 60
#define GRAPH_NUM_SAMPLES 11
#define GRAPH_X_STEP (GRAPH_WIDTH / (GRAPH_NUM_SAMPLES - 1))
#define MAX_POWER_MW 1000.0f

static float power_mW;
static float temperature;
static uint16_t light_adc;
static uint16_t pot_adc;
static uint8_t humidity;
static const char* light_status_str;

static uint32_t last_update_tick = 0;
static uint32_t last_alarm_tick = 0;
static uint32_t last_led7_scan_tick = 0;
static uint32_t last_graph_sample_tick = 0;

static uint8_t alarm_active = 0;
static uint8_t buzzer_state = 0;
static uint8_t colon_state = 0;

static float power_history[GRAPH_NUM_SAMPLES];

static uint16_t scale_power_to_y(float power) {
    if (power < 0.0f) power = 0.0f;
    if (power > MAX_POWER_MW) power = MAX_POWER_MW;

    float scaled_val = power / MAX_POWER_MW;
    return GRAPH_START_Y + GRAPH_HEIGHT - (uint16_t)(scaled_val * GRAPH_HEIGHT);
}

static void display_DrawGraph(void) {
    lcd_Fill(GRAPH_START_X + 1, GRAPH_START_Y + 1,
             GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y + GRAPH_HEIGHT - 1,
             BLACK);

    uint16_t x1, y1, x2, y2;

    for (int i = 0; i < (GRAPH_NUM_SAMPLES - 1); i++) {
        x1 = GRAPH_START_X + i * GRAPH_X_STEP;
        y1 = scale_power_to_y(power_history[i]);

        x2 = GRAPH_START_X + (i + 1) * GRAPH_X_STEP;
        y2 = scale_power_to_y(power_history[i+1]);

        lcd_DrawLine(x1, y1, x2, y2, GREEN);
    }
}

static void display_DrawStaticLayout(void) {
    lcd_Clear(BLACK);
    lcd_ShowStr(10, 10, "HE THONG QUAN TRAC", YELLOW, BLACK, 16, 0);
    lcd_ShowStr(170, 10, "TIME:", WHITE, BLACK, 16, 0);

    lcd_StrCenter(0, 160, "TRANG THAI: BINH THUONG", GREEN, BLACK, 16, 0);

    lcd_ShowStr(GRAPH_START_X, GRAPH_START_Y - 12, "Bieu Do Cong Suat (mW)", WHITE, BLACK, 8, 0);
    lcd_DrawRectangle(GRAPH_START_X, GRAPH_START_Y,
                      GRAPH_START_X + GRAPH_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT,
                      WHITE);
}

static void display_ReadAndProcessSensors(void) {
    sensor_Read();
    temperature = sensor_GetTemperature();
    pot_adc = sensor_GetPotentiometer();
    humidity = (uint8_t)((pot_adc / ADC_MAX_VALUE) * 100.0f);
    if (humidity > 100) humidity = 100;

    light_adc = sensor_GetLight();
    if (light_adc > LIGHT_THRESHOLD) {
        light_status_str = "Strong";
    } else {
        light_status_str = "Weak  ";
    }

    power_mW = sensor_GetVoltage() * sensor_GetCurrent();
}

static void display_UpdateLCD(void) {
    ds3231_ReadTime();

    lcd_ShowIntNum(210, 10, ds3231_hours, 2, CYAN, BLACK, 16);
    lcd_ShowStr(226, 10, ":", CYAN, BLACK, 16, 0);
    lcd_ShowIntNum(234, 10, ds3231_min, 2, CYAN, BLACK, 16);
    lcd_ShowStr(250, 10, ":", CYAN, BLACK, 16, 0);
    lcd_ShowIntNum(258, 10, ds3231_sec, 2, CYAN, BLACK, 16);

    lcd_ShowStr(10, 50, "Nhiet do:", WHITE, BLACK, 16, 0);
    lcd_ShowFloatNum(100, 50, temperature, 4, WHITE, BLACK, 16);
    lcd_ShowStr(140, 50, "*C  ", WHITE, BLACK, 16, 0);

    uint16_t humi_color = (humidity > HUMIDITY_THRESHOLD) ? RED : WHITE;
    lcd_ShowStr(10, 75, "Do am:", WHITE, BLACK, 16, 0);
    lcd_ShowIntNum(100, 75, humidity, 3, humi_color, BLACK, 16);
    lcd_ShowStr(132, 75, "%   ", humi_color, BLACK, 16, 0);

    lcd_ShowStr(10, 100, "Anh sang:", WHITE, BLACK, 16, 0);
    lcd_ShowStr(100, 100, (char*)light_status_str, WHITE, BLACK, 16, 0);

    lcd_ShowStr(10, 125, "Cong suat:", WHITE, BLACK, 16, 0);
    lcd_ShowFloatNum(100, 125, power_mW, 7, WHITE, BLACK, 16);
    lcd_ShowStr(150, 125, "mW  ", WHITE, BLACK, 16, 0);

    if (alarm_active) {
        lcd_StrCenter(0, 160, " CANH BAO: DO AM CAO!    ", RED, BLACK, 16, 0);
    } else {
        lcd_StrCenter(0, 160, "TRANG THAI: BINH THUONG", GREEN, BLACK, 16, 0);
    }

    led7_SetDigit(ds3231_hours / 10, 0, 0);
    led7_SetDigit(ds3231_hours % 10, 1, 0);
    led7_SetDigit(ds3231_min / 10, 2, 0);
    led7_SetDigit(ds3231_min % 10, 3, 0);
}

static void display_CheckAlarmState(void) {
    if (humidity > HUMIDITY_THRESHOLD) {
        alarm_active = 1;
    } else {
        if (alarm_active == 1) {
            alarm_active = 0;
            buzzer_SetVolume(0);
            buzzer_state = 0;
        }
    }
}

static void display_ExecuteAlarmAction(void) {
    buzzer_state = !buzzer_state;
    buzzer_SetVolume(buzzer_state ? 50 : 0);
}

void display_Init(void) {
    buzzer_SetVolume(0);
    led7_init();

    for(int i = 0; i < GRAPH_NUM_SAMPLES; i++) {
        power_history[i] = 0.0f;
    }

    display_DrawStaticLayout();

    uint32_t now = HAL_GetTick();
    last_update_tick = now;
    last_alarm_tick = now;
    last_led7_scan_tick = now;
    last_graph_sample_tick = now;
}

void display_Run(void) {
    uint32_t current_tick = HAL_GetTick();

    if (current_tick - last_update_tick >= 500) {
        last_update_tick = current_tick;

        display_ReadAndProcessSensors();
        display_UpdateLCD();
        display_CheckAlarmState();
    }

    if (current_tick - last_alarm_tick >= 1000) {
        last_alarm_tick = current_tick;
        colon_state = !colon_state;

        if (alarm_active) {
            display_ExecuteAlarmAction();
            uart_Rs232SendString((uint8_t*)"ALARM: Do am vuot nguong > 70%!\r\n");
        }
    }

    if (current_tick - last_led7_scan_tick >= 5) {
        last_led7_scan_tick = current_tick;
        led7_SetColon(colon_state);
        led7_Scan();
    }

    if (current_tick - last_graph_sample_tick >= 15000) {
        last_graph_sample_tick = current_tick;

        for (int i = 0; i < (GRAPH_NUM_SAMPLES - 1); i++) {
            power_history[i] = power_history[i+1];
        }
        power_history[GRAPH_NUM_SAMPLES - 1] = power_mW;

        display_DrawGraph();
    }
}
