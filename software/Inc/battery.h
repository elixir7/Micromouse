#ifndef battery_h
#define battery_h

#include "main.h"

void init_voltmeter(ADC_HandleTypeDef* hadc);
float adc_to_voltage(uint8_t adc_value, uint32_t R1, uint32_t R2);
float GET_voltage_battery(void);
float GET_voltage_cell_1(void);
float GET_voltage_cell_2(void);
float GET_voltage_boost(void);

#endif
