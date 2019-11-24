#include "oled.h"
#include "ssd1306.h"
#include "bitmaps.h"
#include "battery.h"

#include <stdio.h>
#include <string.h>

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

// Enumeration for what to render
typedef enum {
    MENU = 0x00, 
    INFO = 0x01,
	IMAGE = 0x02
} OLED_SCREEN;
/* // TODO put general oled info in struct.
typedef struct {
	uint8_t error_occurd;
	uint8_t cursor;
	OLED_SCREEN active_screen;
}oled_t;
*/

OLED_SCREEN active_screen;
const unsigned char * curr_image;
uint8_t error_occurd = 0;
uint8_t cursor = 0;
float revolutions = 0;
struct menu * curr_submenu = NULL;
struct menu * prev_submenu = NULL;
menu_t main_menu;

menu_t sub_menu_1;
menu_item_t menu_item_back_main;

menu_item_t menu_item_11;
menu_item_t menu_item_12;
menu_item_t menu_item_13;


menu_t sub_menu_2;
menu_item_t menu_item_21;
menu_item_t menu_item_22;


menu_t sub_menu_3;
menu_item_t menu_item_31;
menu_item_t menu_item_32;


/**
	* @brief Initialize the oled screen and a welcome screen. 
*/
void oled_init(void){
	ssd1306_Init();
	
	ssd1306_DrawBitmap(jerry_bmp);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	
	// Show main menu
	active_screen = MENU;
	
	// Main Menu
	strcpy(main_menu.name, "Main menu");
	main_menu.head_item = NULL; 	// main menu has no items, only sub menus
	main_menu.pNext  = &sub_menu_1; // Start of submenus
	
	// Back to main menu item
	strcpy(menu_item_back_main.name, "Back");
	menu_item_back_main.pNext = NULL;
	menu_item_back_main.function = go_back_main;
	
	// Sub menu 1 + items
	strcpy(sub_menu_1.name, "General");
	sub_menu_1.pNext = &sub_menu_2;
	sub_menu_1.head_item = &menu_item_11;
	
	strcpy(menu_item_11.name, "Peripheral info");
	menu_item_11.pNext = &menu_item_12;
	menu_item_11.function = oled_show_info;
	
	strcpy(menu_item_12.name, "CASE Logo");
	menu_item_12.pNext = &menu_item_13;
	menu_item_12.function = oled_show_case;
	
	strcpy(menu_item_13.name, "Nyx & Iris");
	menu_item_13.pNext = &menu_item_back_main;
	menu_item_13.function = oled_show_cats;
	
	// Sub menu 2 + items
	strcpy(sub_menu_2.name, "Submenu 2");
	sub_menu_2.pNext = &sub_menu_3;
	sub_menu_2.head_item = &menu_item_21;
	
	strcpy(menu_item_21.name, "Item 21");
	menu_item_21.pNext = &menu_item_22;
	
	strcpy(menu_item_22.name, "Item 22");
	menu_item_22.pNext = &menu_item_back_main;


	// Sub menu 3 + items
	strcpy(sub_menu_3.name, "Submenu 3");
	sub_menu_3.pNext = NULL;
	sub_menu_3.head_item = &menu_item_31;
	
	strcpy(menu_item_31.name, "Item 31");
	menu_item_31.pNext = &menu_item_32;
	
	strcpy(menu_item_32.name, "Item 32");
	menu_item_32.pNext = &menu_item_back_main;
}


/**
	* @brief Update the screen with new data
	*	
	* Will check if errors have occurd and only print an error screen.
*/
void oled_update(){
	// Don't print anything if an error has occurd
	if(error_occurd){
		return;
	}
	
	if(active_screen == MENU){
		oled_menu();
	}else if(active_screen == INFO){
		oled_info_screen();
	}else if(active_screen == IMAGE){
		ssd1306_DrawBitmap(curr_image);
	}
	
	ssd1306_UpdateScreen();
}


/**
	* @brief Show an screen with general info about the mouse peripherals.
*/
void oled_info_screen(){
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
	
	snprintf(buff, sizeof(buff), "Encoder L: %d", -TIM5->CNT);
	ssd1306_SetCursor(0,40);
	ssd1306_WriteString(buff, Font_7x10, White);
	
	snprintf(buff, sizeof(buff), "Encoder R: %d", -TIM2->CNT);
	ssd1306_SetCursor(0,50);
	ssd1306_WriteString(buff, Font_7x10, White);
}



/**
	* @brief Update the screen with a submitted error message.
	*
	* Keep the error messages short and consice, e.g "Low voltage", max 64 characters.
	*
	* @param (char *pMessage) String pointer with the error message. 
*/
void oled_error(char *pMessage){
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	
	ssd1306_Fill(White);
	ssd1306_SetCursor((128-5*16)/2, 0);
	ssd1306_WriteString("ERROR", Font_16x26, Black);
	
	ssd1306_SetCursor(0, 30);
	ssd1306_WriteString(pMessage, Font_7x10, Black);
	
	ssd1306_SetCursor(0, 50);
	ssd1306_WriteString("Press reset", Font_7x10, Black);

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




uint8_t oled_nr_items(void){
	uint8_t nr_items = 0;
	
	if(curr_submenu == NULL){
		struct menu * sub_menu = main_menu.pNext;
		while(sub_menu != NULL){
			nr_items++;
			sub_menu = sub_menu->pNext;
		}
	}else{
		struct menu_item * item = curr_submenu->head_item;
		while(item != NULL){
			nr_items++;
			item = item->pNext;
		}
	}
	
	return nr_items;
}

/**
	* @brief Show the menu screen with a cursor
	* 
	* The cursor position can be changed by rotating the wheels
	* You can choose menu or function by pressing the "Select Button" on the board 
	*
*/
void oled_menu(void){
	// Calculate cursor index based on encoder
	revolutions = (-TIM2->CNT / (float)(2048 * 60/16)) * 6;
	cursor = (int) revolutions % oled_nr_items(); // Should be calculated based on acctual number of items in the current sub_menu
	
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





/**
	* @brief Go back to the main menu
*/
void go_back_main(void){
	curr_submenu = NULL;
    prev_submenu = NULL;
}



/**
	* @brief Update the menu when a button is pressed
	*
	* The menu will change based on where the cursor is
*/
void oled_button_press(void){
	if(active_screen != MENU){
		active_screen = MENU;
		return; // Don't continue checking if we are on another screen.
	}
	
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


// Menu item funcions
void oled_show_case(void){
	active_screen = IMAGE;
	curr_image = case_head_bmp;
}
void oled_show_cats(void){
	active_screen = IMAGE;
	curr_image = cats_bmp;
}
void oled_show_info(void){
	active_screen = INFO;
}






