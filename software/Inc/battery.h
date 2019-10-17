#ifndef battery_h
#define battery_h

#include "main.h"

typedef enum{
	BATTERY_OK = 0,
	BATTERY_LOW_VOLTAGE = 1,
	BATTERY_OVER_VOLTAGE = 2,
} battery_status_t;

void voltmeter_init(void);
float adc_to_voltage(uint8_t adc_value, uint32_t R1, uint32_t R2);
float GET_voltage_battery(void);
float GET_voltage_cell_1(void);
float GET_voltage_cell_2(void);
float GET_voltage_boost(void);

battery_status_t voltage_check(void);

#endif
