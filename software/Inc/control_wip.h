#ifndef control_wip_h
#define control_wip_h

#include "main.h"

void controller_init(void);
void control_loop(void);
void update_encoder(void);
void update_current_speed(void);
void update_motor_pwm(void);
void update_sensor_error(void);
int  need_to_decelerate(int32_t distanceLeft, int16_t currentSpeed, int16_t endSpeed);
int speed_to_counts(float speed);
float counts_to_speed(int counts);


#endif
