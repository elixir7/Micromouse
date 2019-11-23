#include "motor.h"
#include "main.h"
#include "tim.h"

/**
	*	@brief Initialize motor periphirals
*/
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


/**
	* @brief Limits the speed to the maximum values allowed
	*
	* @param (int speed) The speed about to be checked and capped 
	* @return A capped speed
	* @retval int in range [-255, 255]
*/
int limit_speed(int speed){
	if(speed > 255){
		speed = 255;
	}else if(speed < -255){
		speed = -255;
	}
	return speed;
}


/**
	* @brief Set the speed of the left motor
	*
	*	The speed is controlled by varying the duty cycle or the switching mosfets in the TB6612FNG motor driver.
	* A value of -255/255 will cause the duty cycle to be maxed out meaning full speed in backward/forward.
	* A value of 0 means it will stop.
	*
	* @param (int speed) The desired speed
	*
*/
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

/**
	* @brief Set the speed of the right motor
	*
	*	The speed is controlled by varying the duty cycle or the switching mosfets in the TB6612FNG motor driver.
	* A value of -255/255 will cause the duty cycle to be maxed out meaning full speed in backward/forward.
	* A value of 0 means it will stop.
	*
	* @param (int speed) The desired speed
	*
*/
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

/**
	* @brief Stop both motors by reducing speed to zero
*/
void motors_stop(void){
	SET_PWM_L(0);
	SET_PWM_R(0);
}

