#include "display.h"
#include <stdlib.h>

#define BUTTON_SCAN_INTERVAL_MS 1
#define HOLD_TIME_MS 80
#define REPEAT_TIME_MS 8
#define HOLD_THRESHOLD (HOLD_TIME_MS / BUTTON_SCAN_INTERVAL_MS)
#define REPEAT_RATE (REPEAT_TIME_MS / BUTTON_SCAN_INTERVAL_MS)

uint8_t hour = 0;
uint8_t min = 0;
uint8_t sec = 0;
uint8_t year = 0;
uint8_t month = 0;
uint8_t date = 0;
uint8_t day = 0;

uint8_t a_hour = 0;
uint8_t a_min = 0;
uint8_t a_sec = 0;
uint8_t alarmFlag = 0;

uint8_t count = 0;
uint8_t flag = 1;
uint8_t mode = 0;
uint8_t buf = 0;
uint8_t position = 0;

static uint8_t update_step = 0;
static uint8_t uart_buf[20];
static uint8_t uart_idx = 0;

void initDisplay(int h,int mi, int s, int d,int dd,int m, int y){
	hour = h; min = mi; sec = s, day = d, date = dd; month = m, year = y;
	updateTime();
}

void updateTime(){
	ds3231_Write(ADDRESS_YEAR, year);
	ds3231_Write(ADDRESS_MONTH, month);
	ds3231_Write(ADDRESS_DATE, date);
	ds3231_Write(ADDRESS_DAY, day);
	ds3231_Write(ADDRESS_HOUR, hour);
	ds3231_Write(ADDRESS_MIN, min);
	ds3231_Write(ADDRESS_SEC, sec);
}
void blinkyDisplay(uint8_t val,uint8_t pos){
	count++;
	if(count == 10){
		count = 0;
		switch (pos){
		case 0:
			if(flag) lcd_ShowIntNum(70, 100, val, 2, GREEN, BLACK, 24);
			else lcd_ShowIntNum(70, 100, val, 2, BLACK, BLACK, 24);
			break;
		case 1:
			if(flag) lcd_ShowIntNum(110, 100, val, 2, GREEN, BLACK, 24);
			else lcd_ShowIntNum(110, 100, val, 2, BLACK, BLACK, 24);
			break;
		case 2:
			if(flag) lcd_ShowIntNum(150, 100, val, 2, GREEN, BLACK, 24);
			else lcd_ShowIntNum(150, 100, val, 2, BLACK, BLACK, 24);
			break;
		case 3:
			if(flag) lcd_ShowIntNum(20, 130, val, 2, YELLOW, BLACK, 24);
			else lcd_ShowIntNum(20, 130, val, 2, BLACK, BLACK, 24);
			break;
		case 4:
			if(flag) lcd_ShowIntNum(70, 130, val, 2, YELLOW, BLACK, 24);
			else lcd_ShowIntNum(70, 130, val, 2, BLACK, BLACK, 24);
			break;
		case 5:
			if(flag) lcd_ShowIntNum(110, 130, val, 2, YELLOW, BLACK, 24);
			else lcd_ShowIntNum(110, 130, val, 2, BLACK, BLACK, 24);
			break;
		case 6:
			if(flag) lcd_ShowIntNum(150, 130, val, 2, YELLOW, BLACK, 24);
			else lcd_ShowIntNum(150, 130, val, 2, BLACK, BLACK, 24);
		default:
			break;
		}
		flag = !flag;
	}
}
void displayTime(){
	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}
void displayConfigTime(){
	lcd_ShowIntNum(70, 100, hour, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, year, 2, YELLOW, BLACK, 24);
}
void displayAlarmTime(){
	lcd_ShowIntNum(70, 100, a_hour, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, a_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, a_sec, 2, GREEN, BLACK, 24);
}
void configMode(){
	ds3231_ReadTime();

	if(button_count[3] == 1) {
		++buf;
	} else if (button_count[3] > HOLD_THRESHOLD) {
		if ((button_count[3] - HOLD_THRESHOLD) % REPEAT_RATE == 0) {
			++buf;
		}
	}

	if(button_count[7] == 1){
		++mode;buf = 0;position = 0; displayAlarmTime();
	}

	if (button_count[11] == 1) {
		buf = 0;
		position++;
		position = position % 7;
		displayConfigTime();
	}

	if (button_count[12] == 1){
		switch (position){
		case 0:
			hour = (hour + buf) % 24;
			break;
		case 1:
			min = (min + buf) % 60;
			break;
		case 2:
			sec = (sec + buf) % 60;
			break;
		case 3:
			day = (day + buf) % 7;
			break;
		case 4:
			date = ((date - 1 + buf) % 31) + 1;
			break;
		case 5:
			month = ((month - 1 + buf) % 12) + 1;
			break;
		case 6:
			year += buf;
			break;
		default:
			break;
		}
		updateTime();
		buf = 0;
	}

	switch (position){
	case 0:
		blinkyDisplay((hour + buf) % 24, 0);
		break;
	case 1:
		blinkyDisplay((min + buf) % 60, 1);
		break;
	case 2:
		blinkyDisplay((sec + buf) % 60, 2);
		break;
	case 3:
		blinkyDisplay((day + buf) % 7, 3);
		break;
	case 4:
		blinkyDisplay(((date - 1 + buf) % 31) + 1, 4);
		break;
	case 5:
		blinkyDisplay(((month - 1 + buf) % 12) + 1, 5);
		break;
	case 6:
		blinkyDisplay(year + buf, 6);
	default:
		break;
	}
}

void setAlarm(){
	ds3231_ReadTime();

	uint8_t should_increment = 0;
	if(button_count[3] == 1) {
		should_increment = 1;
	} else if (button_count[3] > HOLD_THRESHOLD) {
		if ((button_count[3] - HOLD_THRESHOLD) % REPEAT_RATE == 0) {
			should_increment = 1;
		}
	}

	if(should_increment) {
		switch (position){
		case 0:
			a_hour = (a_hour + 1) % 24;
			break;
		case 1:
			a_min = (a_min + 1) % 60;
			break;
		case 2:
			a_sec = (a_sec + 1) % 60;
			break;
		default:
			break;
		}
	}

	if(button_count[7] == 1){
		mode = 3;buf = 0;position = 0;
	}

	if (button_count[11] == 1) {
		buf = 0;
		position++;
		position = position % 3;
		displayAlarmTime();
	}

	if (button_count[12] == 1){
		alarmFlag = 1;
	}

	switch (position){
	case 0:
		blinkyDisplay((a_hour) % 24, 0);
		break;
	case 1:
		blinkyDisplay((a_min) % 60, 1);
		break;
	case 2:
		blinkyDisplay((a_sec) % 60, 2);
		break;
	default:
		break;
	}
}

void checkAlarm(){
	if(alarmFlag){
		if(a_hour == ds3231_hours && a_min == ds3231_min && a_sec == ds3231_sec){
			lcd_Fill(0, 180, 240, 220, RED);
			lcd_ShowStr(40, 190, (uint8_t*)"***ALARM***", WHITE, RED, 24, 0);
		}
		if(button_count[12]){
			lcd_Fill(0, 180, 240, 220, BLACK);
			alarmFlag = 0;
		}
	}
}
static int8_t read_uart_line(void) {
    while (uart_data_available() > 0) {
        int16_t c = uart_read_byte();
        if (c == -1) continue;

        if (c == '\b' || c == 127) {
            if (uart_idx > 0) {
                uart_idx--;
                uart_Rs232SendBytes((uint8_t*)"\b \b", 3);
            }
            continue;
        }

//        uart_Rs232SendBytes((uint8_t*)&c, 1);

        if (c == '\r' || c == '\n') {
            uart_Rs232SendString((uint8_t*)"\r\n");
            if (uart_idx > 0) {
                uart_buf[uart_idx] = '\0';
                uart_idx = 0;
                return 1;
            }
        }
        else if (c >= '0' && c <= '9') {
            if (uart_idx < 19) {
                uart_buf[uart_idx++] = (uint8_t)c;
            }
        }
    }
    return 0;
}

void uartUpdateMode(void) {
    if (button_count[7] == 1) {
        mode = 0;
        update_step = 0;
        lcd_Fill(0, 40, 240, 220, BLACK);
        uart_Rs232SendString((uint8_t*)"\r\nUpdate Canceled.\r\n");
        return;
    }

    uint8_t val;

    switch (update_step) {
    case 0:
        lcd_Fill(0, 40, 240, 220, BLACK);
        uart_Rs232SendString((uint8_t*)"\r\n--- UART Time Update Mode ---\r\n");
        update_step = 1;
        break;

    case 1:
        lcd_ShowStr(10, 40, (uint8_t*)"Updating hours ... ", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Hours (0-23): ");
        uart_idx = 0;
        update_step = 2;
        break;
    case 2:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val < 24) hour = val;
            lcd_ShowIntNum(200, 40, hour, 2, GREEN, BLACK, 16);
            update_step = 3;
        }
        break;

    case 3:
        lcd_ShowStr(10, 60, (uint8_t*)"Updating minutes ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Minutes (0-59): ");
        update_step = 4;
        break;
    case 4:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val < 60) min = val;
            lcd_ShowIntNum(200, 60, min, 2, GREEN, BLACK, 16);
            update_step = 5;
        }
        break;

    case 5:
        lcd_ShowStr(10, 80, (uint8_t*)"Updating seconds ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Seconds (0-59): ");
        update_step = 6;
        break;
    case 6:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val < 60) sec = val;
            lcd_ShowIntNum(200, 80, sec, 2, GREEN, BLACK, 16);
            update_step = 7;
        }
        break;

    case 7:
        lcd_ShowStr(10, 100, (uint8_t*)"Updating day ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Day (1-7): ");
        update_step = 8;
        break;
    case 8:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val >= 1 && val <= 7) day = val;
            lcd_ShowIntNum(200, 100, day, 2, GREEN, BLACK, 16);
            update_step = 9;
        }
        break;

    case 9:
        lcd_ShowStr(10, 120, (uint8_t*)"Updating date ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Date (1-31): ");
        update_step = 10;
        break;
    case 10:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val >= 1 && val <= 31) date = val;
            lcd_ShowIntNum(200, 120, date, 2, GREEN, BLACK, 16);
            update_step = 11;
        }
        break;

    case 11:
        lcd_ShowStr(10, 140, (uint8_t*)"Updating month ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Month (1-12): ");
        update_step = 12;
        break;
    case 12:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val >= 1 && val <= 12) month = val;
            lcd_ShowIntNum(200, 140, month, 2, GREEN, BLACK, 16);
            update_step = 13;
        }
        break;

    case 13:
        lcd_ShowStr(10, 160, (uint8_t*)"Updating year ...", WHITE, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"Enter Year (0-99): ");
        update_step = 14;
        break;
    case 14:
        if (read_uart_line()) {
            val = (uint8_t)atoi((char*)uart_buf);
            if (val < 100) year = val;
            lcd_ShowIntNum(200, 160, year, 2, GREEN, BLACK, 16);
            update_step = 15;
        }
        break;

    case 15:
        lcd_ShowStr(10, 180, (uint8_t*)"Saving to DS3231...", GREEN, BLACK, 16, 0);
        uart_Rs232SendString((uint8_t*)"\r\nAll values received. Saving to RTC...\r\n");
        updateTime();
        HAL_Delay(1000);
        update_step = 16;
        break;

    case 16:
        uart_Rs232SendString((uint8_t*)"Save complete. Exiting.\r\n");
        mode = 0;
        update_step = 0;
        lcd_Fill(0, 40, 240, 220, BLACK);
        break;
    }
}

void Display(void){

	switch (mode){
	case 0:
		lcd_ShowStr(10, 10, (uint8_t*)"NORMAL ", WHITE, BLACK, 24, 0);
		ds3231_ReadTime();
		displayTime();
		checkAlarm();
		if(button_count[7] == 1){
			mode = 1;
			hour = ds3231_hours; min = ds3231_min; sec = ds3231_sec, day = ds3231_day, date = ds3231_date; month = ds3231_month, year = ds3231_year;
		}
		break;
	case 1:
		lcd_ShowStr(10, 10, (uint8_t*)"CONFIG ", WHITE, BLACK, 24, 0);
		configMode();
		break;
	case 2:
		lcd_ShowStr(10, 10, (uint8_t*)"ALARM  ", WHITE, BLACK, 24, 0);
		setAlarm();
		break;
	case 3:
		lcd_ShowStr(10, 10, (uint8_t*)"UART UPDATE", WHITE, BLACK, 24, 0);
		uartUpdateMode();
		break;
	default:
		mode = 0;
		break;
	}
}
