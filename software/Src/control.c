#include "control.h"
#include "main.h"
#include "motor.h"

// Sensors
float leftSensor = 0;
float rightSensor = 0;
float frontSensor = 0;
float frontSensorTreshhold = 0;
float leftSensorMiddleValue = 0;
float rightSensorMiddleValue = 0;
float sensorError = 0;
float sensorFeedback = 0;;
float sensorScale = 0.1;

//Used for disabling and enabling when the sensors should help guide the mouse and when not to. 
int disableSensorAdjustment = 1;

// Used for control loop
int leftEncoder;
int rightEncoder;
int leftEncoderOld;
int rightEncoderOld;
int leftEncoderChange;
int rightEncoderChange;
int leftEncoderCount;
int rightEncoderCount;
float encoderCount;
float rotationCount;
float oldEncoderCount;
float oldRotationCount;
float encoderChange;
float rotationChange;
float distanceLeft;
float rotationLeft;
int leftBaseSpeed;
int rightBaseSpeed;

float curSpeedX = 0;
float curSpeedW = 0;
int targetSpeedX = 0;
int targetSpeedW = 0;
int encoderFeedbackX = 0;
int encoderFeedbackW = 0;
float rotationalFeedback = 0;
float posErrorX = 0;
float posErrorW = 0;
float oldPosErrorX = 0;
float oldPosErrorW = 0;
int posPwmX = 0;
int posPwmW = 0;
float kpX = 2, kdX = 4;
float kpW = 1, kdW = 10;//used in straight
float accX = 200; // 100 cm/s^2 => 1 m/s^2 
float decX = 200; 
float accW = 2; //cm/s^2
float decW = 2;

int oneCellDistance = distanceToCounts(178);
int quarterTurn = rotToCounts(90);


void speedProfile(void){	
	getEncoderStatus();
	updateCurrentSpeed();
	calculateMotorPwm();      
}


void getEncoderStatus(void)
{
	leftEncoder = - TIM5->CNT;		//read current encoder ticks from register of 32 bit general purpose timer 2
	rightEncoder = - TIM2->CNT;		//read current encoder ticks from register of 32 bit general purpose timer 5

	leftEncoderChange = leftEncoder - leftEncoderOld;
	rightEncoderChange = rightEncoder - rightEncoderOld;
	
	encoderChange = ( rightEncoderChange + leftEncoderChange)/2.0;	 
	rotationChange = (rightEncoderChange - leftEncoderChange)/2.0;

	leftEncoderOld = leftEncoder;
	rightEncoderOld = rightEncoder;
					
	leftEncoderCount += leftEncoderChange;
	rightEncoderCount += rightEncoderChange;
	encoderCount =  (leftEncoderCount+rightEncoderCount)/2.0;	
	rotationCount = (rightEncoderCount - leftEncoderCount)/2.0;
	
	distanceLeft -= encoderChange;
	rotationLeft -= rotationChange;
}



void updateCurrentSpeed(void)
{
	if(curSpeedX < targetSpeedX)
	{
		curSpeedX += (float)(speedToCounts(accX*2)/100);
		if(curSpeedX > targetSpeedX)
			curSpeedX = targetSpeedX;
	}
	else if(curSpeedX > targetSpeedX)
	{
		curSpeedX -= (float)speedToCounts(decX*2)/100;
		if(curSpeedX < targetSpeedX)
			curSpeedX = targetSpeedX;
	}
	if(curSpeedW < targetSpeedW)
	{
		curSpeedW += accW;
		if(curSpeedW > targetSpeedW)
			curSpeedW = targetSpeedW;
	}
	else if(curSpeedW > targetSpeedW)
	{
		curSpeedW -= decW;
		if(curSpeedW < targetSpeedW)
			curSpeedW = targetSpeedW;
	}	
}


void calculateMotorPwm(void) // Position and rotation PD controller
{	
	
	// PD loop for translation and rotation to generate base speed for both motors
	encoderFeedbackX = rightEncoderChange + leftEncoderChange;
	encoderFeedbackW = rightEncoderChange - leftEncoderChange;	
	 
	// Have sensor error properly scale to fit the system
	//sensorFeedback = sensorError / sensorScale; 
	
	if(disableSensorAdjustment)
		sensorFeedback = 0;
	
	rotationalFeedback = encoderFeedbackW + sensorFeedback;

	posErrorX = curSpeedX - encoderFeedbackX;
	posErrorW = curSpeedW - rotationalFeedback;
	
	posPwmX = kpX * posErrorX + kdX * (posErrorX - oldPosErrorX);
	posPwmW = kpW * posErrorW + kdW * (posErrorW - oldPosErrorW);
	
	oldPosErrorX = posErrorX;
	oldPosErrorW = posErrorW;
	
	leftBaseSpeed = posPwmX - posPwmW;
	rightBaseSpeed = posPwmX + posPwmW;

	SET_PWM_L(leftBaseSpeed);
	SET_PWM_R(rightBaseSpeed);
}

int needToDecelerate(int32_t dist, int16_t curSpd, int16_t endSpd)//speed are in encoder counts/ms, dist is in encoder counts 
{
	if (curSpd<0) curSpd = -curSpd;
	if (endSpd<0) endSpd = -endSpd;
	if (dist<0) dist = -dist;			//-dist;
	if (dist == 0) dist = 1;  //prevent divide by 0

	int a = countsToSpeed((curSpd*curSpd - endSpd*endSpd)*100/dist/4/2);
	
	if(a < 0)
		a = -a;
	
	
	return a;
	
	
	//calculate deceleration rate needed with input distance, input current speed and input targetspeed to determind if the deceleration is needed
	//use equation 2*a*S = Vt^2 - V0^2  ==>  a = (Vt^2-V0^2)/2/S
	//because the speed is the sum of left and right wheels(which means it's doubled), that's why there is a "/4" in equation since the square of 2 is 4
}













