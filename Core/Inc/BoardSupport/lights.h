/*
 * lights.h
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */

#ifndef INC_LIGHTS_H_
#define INC_LIGHTS_H_

#include "gpio.h"
#include <stdint.h>

typedef enum light_status
{
	LIGHT_OFF = 0,
	LIGHT_ON
}light_status_t;

typedef enum light_power_status
{
	LIGHT_POWER_STATUS_ENERGIZED_OFF = 0,
	LIGHT_POWER_STATUS_ENERGIZED_ON,
	LIGHT_POWER_STATUS_DEENERGIZED
}light_power_status_t;

typedef enum light_fsm_state
{
	LIGHT_STATE_IDLE = 0,
	LIGHT_STATE_CHANGE_STATUS,
	LIGHT_STATE_WAIT,
	LIGHT_STATE_DEENERGIZING,
}light_fsm_state_t;

typedef enum light_fsm_status
{
	LIGHT_STATE_STATUS_READY = 0,
	LIGHT_STATE_STATUS_BUSY
}light_fsm_status_t;

typedef enum light_fsm_run
{
	LIGHT_RUN_TRUE = 0,
	LIGHT_RUN_FALSE
}light_fsm_run_t;

typedef enum light_fsm_init
{
	LIGHT_INIT_TRUE = 0,
	LIGHT_INIT_FALSE
}light_fsm_init_t;

typedef struct light_gpio
{
	GPIO_TypeDef *port;
	uint16_t pin;
}light_gpio_t;

typedef struct light
{
	light_status_t light_status;
//	light_status_t new_light_status;
	light_power_status_t power_status;
	light_gpio_t hardware_output_1;
	light_gpio_t hardware_output_2;

	light_fsm_status_t fsm_status;
	uint32_t fsm_pulse_counts;//depends off the time mechanism
	uint32_t fsm_pulse_idx;//depends off the time mechanism
	light_fsm_state_t fsm_state;
	light_fsm_run_t fsm_run_on;
	light_fsm_run_t fsm_run_off;
	light_fsm_init_t fsm_init;


}light_t;

uint8_t light_pulse_fsm(light_t *light, light_status_t new_status);
uint8_t light_on_pulse_fsm(light_t *light);
uint8_t light_off_pulse_fsm(light_t *light);
uint8_t light_setup(light_t *light,light_gpio_t hardware_output_1,
											light_gpio_t hardware_output_2);
uint8_t light_ask_on_pulse_fsm(light_t *light);
uint8_t light_check_init_fsm(light_t light, light_fsm_init_t *init);
uint8_t light_acknowledge_init_fsm(light_t *light);
uint8_t light_ask_off_pulse_fsm(light_t *light);
uint8_t light_off(light_t *light);
uint8_t light_on(light_t *light);
uint8_t light_deenergize(light_t *light);
uint8_t light_get_status(light_t light, light_status_t *status);

#endif /* INC_LIGHTS_H_ */
