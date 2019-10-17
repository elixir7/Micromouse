#include "oled.h"
#include "ssd1306.h"
#include "battery.h"


#include <stdio.h>
uint8_t error_occurd = 0;


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
	if(error_occurd){
		return;
	}
	
	char buff[60];
	
	ssd1306_Fill(Black);
	
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

void oled_error(char *pMessage){
	error_occurd = 1;
	
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	
	ssd1306_Fill(White);
	ssd1306_SetCursor((128-5*16)/2, 0);
	ssd1306_WriteString("ERROR", Font_16x26, Black);
	
	ssd1306_SetCursor(0, 30);
	ssd1306_WriteString(pMessage, Font_7x10, Black);

	ssd1306_UpdateScreen();
}

void oled_clear_error(void){
	error_occurd = 0;
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
	
	oled_update();
}

