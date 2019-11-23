#include "oled.h"
#include "ssd1306.h"
#include "bitmaps.h"
#include "battery.h"

#include <stdio.h>
#include <string.h>

uint8_t error_occurd = 0;
//uint8_t menu_index = 0;

typedef struct menu_item{
	char name[20];
	void (*function)();
	struct menu_item * pNext;
}menu_item_t;


typedef struct menu{
    char name[20];
    struct menu_item * head_item; 	// Menu item start (Only for menu with items)
    struct menu * pNext; 						// Next menu item (Only for main menu)
}menu_t;

uint8_t cursor = 0;
float revolutions = 0;
struct menu * curr_submenu = NULL;
struct menu * prev_submenu = NULL;
menu_t main_menu;

menu_t sub_menu_1;
menu_item_t menu_item_11;
menu_item_t menu_item_12;
menu_item_t menu_item_13;

menu_t sub_menu_2;
menu_item_t menu_item_21;
menu_item_t menu_item_22;
menu_item_t menu_item_23;

menu_t sub_menu_3;
menu_item_t menu_item_31;
menu_item_t menu_item_32;
menu_item_t menu_item_33;

void toggle_led(void){
	HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
}
/*
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
*/

/**
	* @brief Initialize the oled screen and a welcome screen. 
*/
void oled_init(void){
	ssd1306_Init();
	/*
	ssd1306_Fill(Black);
	
	ssd1306_SetCursor((128-5*16)/2, (64-26)/2);
	ssd1306_WriteString("JERRY", Font_16x26, White);
	ssd1306_UpdateScreen();
	*/
	ssd1306_DrawBitmap(jerry_bmp);
	HAL_Delay(1000);
	
	// Setup all menus
	strcpy(main_menu.name, "Main menu");
	main_menu.head_item = NULL; // main has no items, only sub menus
	main_menu.pNext  = &sub_menu_1; // Start of submenus
	
	strcpy(sub_menu_1.name, "Submenu 1");
	sub_menu_1.pNext = &sub_menu_2;
	sub_menu_1.head_item = &menu_item_11;
	strcpy(menu_item_11.name, "Item 11");
	menu_item_11.pNext = &menu_item_12;
	menu_item_11.function = toggle_led;
	strcpy(menu_item_12.name, "Item 12");
	menu_item_12.pNext = &menu_item_13;
	strcpy(menu_item_13.name, "Item 13");
	menu_item_13.pNext = NULL;
	menu_item_13.function = go_back_main;
	
	
	strcpy(sub_menu_2.name, "Submenu 2");
	sub_menu_2.pNext = &sub_menu_3;
	sub_menu_2.head_item = &menu_item_21;
	strcpy(menu_item_21.name, "Item 21");
	menu_item_21.pNext = &menu_item_22;
	strcpy(menu_item_22.name, "Item 22");
	menu_item_22.pNext = &menu_item_23;
	strcpy(menu_item_23.name, "Item 23");
	menu_item_23.pNext = NULL;
	menu_item_23.function = go_back_main;

	strcpy(sub_menu_3.name, "Submenu 3");
	sub_menu_3.pNext = NULL;
	sub_menu_3.head_item = &menu_item_31;
	strcpy(menu_item_31.name, "Item 31");
	menu_item_31.pNext = &menu_item_32;
	strcpy(menu_item_32.name, "Item 32");
	menu_item_32.pNext = &menu_item_33;
	strcpy(menu_item_33.name, "Item 33");
	menu_item_33.pNext = NULL;
	menu_item_33.function = go_back_main;
	
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
	// Calculate cursor index based on encoder
	revolutions = (-TIM2->CNT / (float)(2048 * 60/16));
	cursor = (int) revolutions % 3;
	
	// Background
	ssd1306_Fill(Black);
	
	if(curr_submenu == NULL){
		ssd1306_SetCursor(0,0);
		ssd1306_WriteString(main_menu.name, Font_11x18, White);
		
		struct menu * sub_menu = main_menu.pNext; // Reference to first submenu
		int line = 0;
		
		while(sub_menu != NULL){
			if(line == cursor){
				ssd1306_SetCursor(0, 18 + line*10);
				ssd1306_WriteString(">", Font_7x10, White);
			}
			ssd1306_SetCursor(14, 18 + line*10);
			ssd1306_WriteString(sub_menu->name, Font_7x10, White);
			line++;
			sub_menu = sub_menu->pNext;
		}
	}else{ // We are in a submenu => draw menu items
		ssd1306_SetCursor(0,0);
		ssd1306_WriteString(curr_submenu->name, Font_11x18, White);
		
		
		struct menu_item * item = curr_submenu->head_item;
		int line = 0;
		while(item != NULL){
			if (line == cursor){
				ssd1306_SetCursor(0, 18 + line*10);
				ssd1306_WriteString(">", Font_7x10, White);
			}
			ssd1306_SetCursor(14, 18 + line*10);
			ssd1306_WriteString(item->name, Font_7x10, White);
			line++;
			item = item->pNext;
		}		
	}
	ssd1306_UpdateScreen();
}




void go_back_main(void){
	curr_submenu = NULL;
    prev_submenu = NULL;
}


/*
void go_back(void){
	curr_submenu = prev_submenu;
    prev_submenu = prev_submenu->pParent;
}
*/

/**
	* @brief Update the menu when a button is pressed
	*
	* The menu will change based on where the cursor is
*/
void oled_button_press(void){
	if(curr_submenu == NULL){
        struct menu * sub_menu = main_menu.pNext;
        int line = 0;
        while(sub_menu != NULL){
            if (line == cursor){
				curr_submenu = sub_menu;
				break;
			}
            line++;
            sub_menu = sub_menu->pNext;
		}
    }else{
        struct menu_item * item = curr_submenu->head_item;
        int line = 0;
        while(item != NULL){
            if (line == cursor){
				if(item->function != NULL){
					item->function();
				}
				break;
			}
            line++;
            item = item->pNext;
		}
    }	
}
/*
void oled_menu(void){
	// Calculate index bases on encoders
	revolutions = (-TIM2->CNT / (float)(2048 * 60/16));
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
*/


