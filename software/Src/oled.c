#include "oled.h"
#include "ssd1306.h"
#include "battery.h"

#include <stdio.h>
#include <string.h>

uint8_t error_occurd = 0;
uint8_t menu_index = 0;
uint8_t cursor_index = 0;

char* main_menu[] = {
    "MAIN",
    "Search run",
    "Calibration",
    "Settings"
};

char* calibration_menu[] = {
    "CALIBRATION",
    "Sensors",
    "Gyro",
    "Back"
};

char* settings_menu[] = {
    "SETTINGS",
    "PID",
    "Algorithm",
    "Back"
};

char **menus[] = {
    main_menu,
    calibration_menu,
    settings_menu
};

const uint8_t nr_menus = sizeof(menus) / sizeof(menus[0]);


/**
	* @brief Initialize the oled screen and a welcome screen. 
*/
void oled_init(void){
	ssd1306_Init();
	
	ssd1306_Fill(Black);
	
	ssd1306_SetCursor((128-5*16)/2, (64-26)/2);
	ssd1306_WriteString("JERRY", Font_16x26, White);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
}


/**
	* @brief Update the screen with new data
	*	
	* Will check if errors have occurd and only print an error screen.
*/
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



/**
	* @brief Update the screen with a submitted error message.
	*
	* Keep the error messages short and consice, e.g "Low voltage".
	*
	* @param (char *pMessage) String pointer with the error message. 
*/
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


/**
	* @brief Clear the error and update the screen with normal information.
*/
void oled_clear_error(void){
	error_occurd = 0;
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
	
	oled_update();
}


/**
	* @brief Show the menu screen with a cursor
	* 
	* The cursor position can be changed by rotating the wheels
	* You can choose menu by pressing the "Select Button" on the board 
	*
*/
void oled_menu(void){
	// Calculate index bases on encoders
	float revolutions = (TIM2->CNT / (float)(1024 * 60/16) * 4);
	cursor_index = (int) revolutions % 3;
	
	// Background
	ssd1306_Fill(Black);
	
	// Menu name on top
	ssd1306_SetCursor(0,0);
	ssd1306_WriteString(* menus[menu_index], Font_11x18, White);

	// Menu items
	for(int i = 1; i < 4; i++){
		if(i - 1 == cursor_index){
			ssd1306_SetCursor(0, 18 + (i-1)*10);
			ssd1306_WriteString(">", Font_7x10, White);
		}
		
		ssd1306_SetCursor(14, 18 + (i-1)*10);
		ssd1306_WriteString(* (menus[menu_index] + i), Font_7x10, White);
	}
	
	ssd1306_UpdateScreen();
}

/**
	* @brief Update the menu when a button is pressed
	*
	* The menu will change based on where the cursor is
*/
void oled_button_press(void){
	
	if( strcmp(* (menus[menu_index] + cursor_index ), "Back") ){
		menu_index = 0;
	}else{
		menu_index = cursor_index;
	}
	
	
}
