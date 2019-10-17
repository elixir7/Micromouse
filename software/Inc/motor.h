#ifndef motor_h
#define motor_h

#include "main.h"

void motors_init(void);

void SET_PWM_L(int speed);
void SET_PWM_R(int speed);

void motors_stop(void);

#endif
