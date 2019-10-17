#include "battery.h"
#include "main.h"
#include "adc.h"

// Private array for the DMA to fill with values
#define SAMPLES 64
#define NR 3

uint8_t adc_readings[3];
uint8_t adc_averaged[NR*SAMPLES];
uint32_t nr_indices = (sizeof(adc_averaged) / sizeof(adc_averaged[0]));
uint8_t average;

uint8_t adc_average(uint8_t index){
	// This can be drastically improved by sorting first and then removing a couple of the highest and lowest readings
	uint32_t sum = 0;
	for(int i = index; i < NR*SAMPLES; i += NR){
		sum += adc_averaged[i];
	}
	
	average = sum / SAMPLES;
	return average;
}

void voltmeter_init(void){
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adc_averaged, nr_indices);
}

float adc_to_voltage(uint8_t adc_value, uint32_t R1, uint32_t R2){
	//1. Calculate the voltage between R2 and ground (V_out), (reading / resolution) * V_ref
	//2. Calculate acctual voltage (V_in) with the voltage divider. (V_out = V_in * R2/(R1 + R2) or (V_in = V_out * (R1 + R2) / R2
	return (adc_value / 256.0f) * 3.3f * (R1 + R2) / (float)R2;
}

float GET_voltage_battery(){
	return adc_to_voltage(adc_average(0), 20000, 10000);
}

float GET_voltage_cell_1(void){
	return adc_to_voltage(adc_average(1), 20000, 10000);
}
float GET_voltage_cell_2(void){
	return GET_voltage_battery() - GET_voltage_cell_1();
}

float GET_voltage_boost(void){
	return adc_to_voltage(adc_average(2), 47000, 10000);
}

battery_status_t voltage_check(void){
	battery_status_t status = BATTERY_OK;
	if(GET_voltage_cell_1() > 4.2f || GET_voltage_cell_2() > 4.2f){
		status = BATTERY_OVER_VOLTAGE;
	}else if(GET_voltage_cell_1() < 3.3f || GET_voltage_cell_2() > 3.3f ){
		status = BATTERY_LOW_VOLTAGE;
	}
	return status;
}
