#include "oled.h"
#include "ssd1306.h"
#include "battery.h"


#include <stdio.h>


void oled_init(void){
	ssd1306_Init();
	
	ssd1306_Fill(White);
	ssd1306_SetCursor((128-7*16)/2, (64-26)/2);
	ssd1306_WriteString("Loading", Font_16x26, Black);
	ssd1306_UpdateScreen();
}

void oled_jerry(void){
	ssd1306_Fill(Black);
	
	ssd1306_SetCursor((128-5*16)/2, (64-26)/2);
	ssd1306_WriteString("JERRY", Font_16x26, White);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
}

void oled_update(){
	ssd1306_Fill(Black);
	char buff[60];
	
	snprintf(buff, sizeof(buff), "Battery: %.2f V", GET_voltage_battery());
	ssd1306_SetCursor(0,0);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Cell 1: %.2f V", GET_voltage_cell_1());
	ssd1306_SetCursor(0,10);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Cell 2: %.2f V", GET_voltage_cell_2());
	ssd1306_SetCursor(0,20);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Boost: %.2f V", GET_voltage_boost());
	ssd1306_SetCursor(0,30);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Encoder L: %d", TIM5->CNT);
	ssd1306_SetCursor(0,40);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Encoder R: %d", TIM2->CNT);
	ssd1306_SetCursor(0,50);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	ssd1306_UpdateScreen();
}

