/*
 * relay_l114fl_drv8210.h
 *
 *  Created on: Aug 29, 2024
 *      Author: Eduardo
 */

#ifndef INC_RELAY_L114FL_DRV8210_H_
#define INC_RELAY_L114FL_DRV8210_H_



#include "gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum relay_status
{
	RELAY_OFF = 0,
	RELAY_ON
}relay_status_t;

typedef enum relay_power_status
{
	RELAY_POWER_STATUS_ENERGIZED_OFF = 0,
	RELAY_POWER_STATUS_ENERGIZED_ON,
	RELAY_POWER_STATUS_DEENERGIZED
}relay_power_status_t;

typedef enum relay_fsm_state
{
	RELAY_STATE_IDLE = 0,
	RELAY_STATE_CHANGE_STATUS,
	RELAY_STATE_WAIT,
	RELAY_STATE_DEENERGIZING,
}relay_fsm_state_t;

typedef enum relay_fsm_status
{
	RELAY_STATE_STATUS_READY = 0,
	RELAY_STATE_STATUS_BUSY
}relay_fsm_status_t;

typedef enum relay_fsm_run
{
	RELAY_RUN_TRUE = 0,
	RELAY_RUN_FALSE
}relay_fsm_run_t;

typedef enum relay_fsm_init
{
	RELAY_INIT_TRUE = 0,
	RELAY_INIT_FALSE
}relay_fsm_init_t;

typedef struct relay_gpio
{
	GPIO_TypeDef *port;
	uint16_t pin;
}relay_gpio_t;

typedef struct relay
{
	relay_status_t relay_status;
//	relay_status_t new_relay_status;
	relay_power_status_t power_status;
	relay_gpio_t hardware_output_1;
	relay_gpio_t hardware_output_2;

	relay_fsm_status_t fsm_status;
	uint32_t fsm_pulse_counts;//depends off the time mechanism
	uint32_t fsm_pulse_idx;//depends off the time mechanism
	relay_fsm_state_t fsm_state;
	relay_fsm_run_t fsm_run_on;
	relay_fsm_run_t fsm_run_off;
	relay_fsm_init_t fsm_init;


}relay_t;

uint8_t relay_pulse_fsm(relay_t *relay, relay_status_t new_status);
uint8_t relay_on_pulse_fsm(relay_t *relay);
uint8_t relay_off_pulse_fsm(relay_t *relay);
uint8_t relay_setup(relay_t *relay,relay_gpio_t hardware_output_1,
											relay_gpio_t hardware_output_2);
uint8_t relay_ask_on_pulse_fsm(relay_t *relay);
uint8_t relay_check_init_fsm(relay_t relay, relay_fsm_init_t *init);
uint8_t relay_acknowledge_init_fsm(relay_t *relay);
uint8_t relay_ask_off_pulse_fsm(relay_t *relay);
uint8_t relay_off(relay_t *relay);
uint8_t relay_on(relay_t *relay);
uint8_t relay_deenergize(relay_t *relay);
uint8_t relay_get_status(relay_t relay, relay_status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* INC_RELAY_L114FL_DRV8210_H_ */
