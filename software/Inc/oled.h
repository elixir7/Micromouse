#ifndef oled_h
#define oled_h

#include "main.h"


void oled_init(void);
void oled_update(void);
void oled_error(char *pMessage);
void oled_clear_error(void);
void oled_menu(void);
void oled_button_press(void);


#endif
