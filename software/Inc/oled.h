#ifndef oled_h
#define oled_h

#include "main.h"




void oled_init(void);
void oled_update(void);
void oled_error(char *pMessage);
void oled_clear_error(void);
void oled_button_press(void);
void oled_info_screen(void);
void oled_menu(void);


// Private
void go_back(void);
void go_back_main(void);
uint8_t nr_items(void);

// Menu item functions
void oled_show_case(void);
void oled_show_cats(void);
void oled_show_info(void);

#endif
