#include "motor.h"
#include "main.h"
#include "tim.h"

void motors_init(void){
	// Start encoders
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	
	// Start PWM generation and choose direction on TB6612FBNG driver
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4); // Motor right
	HAL_GPIO_WritePin(MOTOR_R_IN1_GPIO_Port, MOTOR_R_IN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_R_IN2_GPIO_Port, MOTOR_R_IN2_Pin, GPIO_PIN_SET);
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);// Motor left
	HAL_GPIO_WritePin(MOTOR_L_IN1_GPIO_Port, MOTOR_L_IN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MOTOR_L_IN2_GPIO_Port, MOTOR_L_IN2_Pin, GPIO_PIN_RESET);
}

int limit_speed(int speed){
	if(speed > 999){
		speed = 999;
	}else if(speed < -999){
		speed = -999;
	}
	return speed;
}

void SET_PWM_L(int speed){
	int ok_speed = limit_speed(speed);
	
	if(speed < 0){
		speed = -speed;
		
		HAL_GPIO_WritePin(MOTOR_L_IN1_GPIO_Port, MOTOR_L_IN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_L_IN2_GPIO_Port, MOTOR_L_IN2_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(MOTOR_L_IN1_GPIO_Port, MOTOR_L_IN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_L_IN2_GPIO_Port, MOTOR_L_IN2_Pin, GPIO_PIN_RESET);
	}
	
	TIM4->CCR3 = ok_speed;
}

void SET_PWM_R(int speed){
	int ok_speed = limit_speed(speed);
	
	if(speed < 0){
		speed = -speed;
		
		HAL_GPIO_WritePin(MOTOR_R_IN1_GPIO_Port, MOTOR_R_IN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_R_IN2_GPIO_Port, MOTOR_R_IN2_Pin, GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(MOTOR_R_IN1_GPIO_Port, MOTOR_R_IN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_R_IN2_GPIO_Port, MOTOR_R_IN2_Pin, GPIO_PIN_SET);
	}
	
	TIM4->CCR4 = ok_speed;
}

void motors_stop(void){
	SET_PWM_L(0);
	SET_PWM_R(0);
}

