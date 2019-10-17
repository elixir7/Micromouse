#ifndef oled_h
#define oled_h

#include "main.h"

extern uint8_t menu_index;

void oled_init(void);
void oled_update(void);
void oled_jerry(void);
void oled_error(char *pMessage);
void oled_clear_error(void);
void oled_menu(void);


#endif
