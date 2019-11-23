#include "control_wip.h"
#include "main.h"


#define WHEEL_TO_WHEEL 80 // Center of wheel to the center of the other wheel. (mm) 
#define WHEEL_D 21 				// Wheel diameter (mm)
#define CPR 2048.0				// Counts per revolution
#define PI 3.141592			
#define GEARING (60.0/16.0)

typedef struct{
	int counts;
	int counts_old;
	int counts_total;
	int counts_change;
}encoder_t;

typedef struct{
	encoder_t left;
	encoder_t right;
	float counts;
	float counts_old;
	float counts_change;
	float rotations;
	float rotation_change;
	float rotation_old;	
}encoders_t;

typedef struct {
	encoders_t encoders;
	float distance_left;
	float rotation_left;
	float current_speed_x;
	float target_speed_x;
	float current_speed_w;
	float target_speed_w;
	float acc_x;
	float acc_w;
	float dec_x;
	float dec_w;
	int left_base_speed;
	int right_base_speed;
}controller_t;

encoder_t encoder_left;
encoder_t encoder_right;
encoders_t encoders;
controller_t controller;


// Functions
void contro_loop(void){	
	update_encoder();
	update_sensor_error();
	update_current_speed();
	update_motor_pwm();      
}

void controller_init(void){
	encoders.left = encoder_left;
	encoders.right = encoder_right;
	controller.encoders = encoders;
}

void update_encoder(void){
	
	encoder_left.counts = TIM2->CNT;	//read current encoder ticks from register of 32 bit general purpose timer 2
	encoder_right.counts = TIM5->CNT; //read current encoder ticks from register of 32 bit general purpose timer 5

	encoder_left.counts_change = encoder_left.counts - encoder_left.counts_old;
	encoder_right.counts_change = encoder_right.counts - encoder_right.counts_old;
	
	encoders.counts_change = (encoder_right.counts_change + encoder_left.counts_change) / 2.0f;
	encoders.rotation_change = (encoder_right.counts_change - encoder_left.counts_change) / 2.0f;

	encoder_left.counts_old = encoder_left.counts;
	encoder_right.counts_old = encoder_right.counts;

	encoder_left.counts_total += encoder_left.counts_change;
	encoder_right.counts_total += encoder_right.counts_change;
	
	encoders.counts = (encoder_left.counts + encoder_right.counts) / 2.0f;
	encoders.rotations = (encoder_right.counts - encoder_left.counts) / 2.0f;
	
	controller.distance_left -= encoders.counts_change;
	controller.rotation_left -= encoders.rotation_change;
}



void update_current_speed(void)
{
	if(controller.current_speed_x < controller.target_speed_x)
	{
		controller.current_speed_x += (float)(speed_to_counts(controller.acc_x*2) / 100);
		if(controller.current_speed_x > controller.target_speed_x)
			controller.current_speed_x = controller.target_speed_x;
	}
	else if(controller.current_speed_x > controller.target_speed_x)
	{
		controller.current_speed_x -= (float)speed_to_counts(controller.dec_x*2)/100;
		if(controller.current_speed_x < controller.target_speed_x)
			controller.current_speed_x = controller.target_speed_x;
	}
	if(controller.current_speed_w < controller.target_speed_w)
	{
		controller.current_speed_w += controller.acc_w;
		if(controller.current_speed_w > controller.target_speed_w)
			controller.current_speed_w = controller.target_speed_w;
	}
	else if(controller.current_speed_w > controller.target_speed_w)
	{
		controller.current_speed_w -= controller.dec_w;
		if(controller.current_speed_w < controller.target_speed_w)
			controller.current_speed_w = controller.target_speed_w;
	}	
}


void update_motor_pwm(void){	
	
	// PD loop for translation and rotation to generate base speed for both motors
	encoderFeedbackX = rightEncoderChange + leftEncoderChange;
	encoderFeedbackW = rightEncoderChange - leftEncoderChange;	
	 
	//Have sensor error properly scale to fit the system
	sensorFeedback = sensorError / sensorScale; 
	
	if(disableSensorAdjustment)
		sensorFeedback = 0;
	
	rotationalFeedback = encoderFeedbackW + sensorFeedback;

	posErrorX += curSpeedX - encoderFeedbackX;
	posErrorW += curSpeedW - rotationalFeedback;
	
	posPwmX = kpX * posErrorX + kdX * (posErrorX - oldPosErrorX);
	posPwmW = kpW * posErrorW + kdW * (posErrorW - oldPosErrorW);
	
	oldPosErrorX = posErrorX;
	oldPosErrorW = posErrorW;
	
	leftBaseSpeed = posPwmX - posPwmW;
	rightBaseSpeed = posPwmX + posPwmW;

	setLeftPWM(leftBaseSpeed);
	setRightPWM(rightBaseSpeed);	
}

void getSensorError(void){
	leftSensor = calcDistances[0];
	frontSensor = calcDistances[1];
	rightSensor = calcDistances[2];
	
	/*
	// Stop the mouse from running when the mouse runts into walls, not tested.
	if((leftSensor < leftSensorMiddleValue || rightSensor < rightSensorMiddleValue) 
		&& frontSensor < frontSensorTreshhold){
			enableControlLoop = 0;
			setLeftPWM(0);
			setRightPWM(0);
		}
	*/
		
	
	if(leftSensor < leftSensorMiddleValue && rightSensor > rightSensorMiddleValue /*&& rightSensor < 10*/)
		if(rightSensor > 8)
			sensorError = leftSensorMiddleValue - leftSensor - 0.5;
		else
			sensorError = leftSensorMiddleValue - leftSensor;
	else if(rightSensor < rightSensorMiddleValue && leftSensor > leftSensorMiddleValue /*&& leftSensor < 10*/)
		if(leftSensor > 8)
			sensorError = rightSensor - rightSensorMiddleValue;
		else
			sensorError = rightSensor - rightSensorMiddleValue - 0.5;
	else
		sensorError = 0;
}

int need_to_decelerate(int32_t dist, int16_t curSpd, int16_t endSpd)//speed are in encoder counts/ms, dist is in encoder counts 
{
	if (curSpd<0) curSpd = -curSpd;
	if (endSpd<0) endSpd = -endSpd;
	if (dist<0) dist = -dist;			//-dist;
	if (dist == 0) dist = 1;  //prevent divide by 0

	int a = counts_to_speed((curSpd*curSpd - endSpd*endSpd)*100/dist/4/2);
	
	if(a < 0)
		a = -a;
	
	
	return a;
	
	
	//calculate deceleration rate needed with input distance, input current speed and input targetspeed to determind if the deceleration is needed
	//use equation 2*a*S = Vt^2 - V0^2  ==>  a = (Vt^2-V0^2)/2/S
	//because the speed is the sum of left and right wheels(which means it's doubled), that's why there is a "/4" in equation since the square of 2 is 4
}



