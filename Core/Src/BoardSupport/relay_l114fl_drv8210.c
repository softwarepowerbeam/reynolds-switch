/*
 * relay_l114fl_drv8210.c
 *
 *  Created on: Aug 29, 2024
 *      Author: Eduardo
 */

#include "BoardSupport/relay_l114fl_drv8210.h"


uint8_t relay_setup(relay_t *relay, relay_gpio_t hardware_output_1,
												relay_gpio_t hardware_output_2)
{

	relay->hardware_output_1 = hardware_output_1;
	relay->hardware_output_2 = hardware_output_2;
	relay_deenergize(relay);
	relay->relay_status = RELAY_OFF;
	relay->fsm_run_on = RELAY_RUN_FALSE;
	relay->fsm_run_off = RELAY_RUN_FALSE;
	relay->fsm_pulse_counts = 1;
	return 0;

}
uint8_t relay_on_pulse_fsm(relay_t *relay)
{
	if( (relay->fsm_run_on == RELAY_RUN_TRUE) &&
		(relay->fsm_run_off != RELAY_RUN_TRUE) )
	{
		relay_pulse_fsm(relay, RELAY_ON);

		if(relay->fsm_status == RELAY_STATE_STATUS_READY)
		{
			relay->fsm_run_on = RELAY_RUN_FALSE;
		}
	}
	return 0;
}

uint8_t relay_off_pulse_fsm(relay_t *relay)
{
	if( (relay->fsm_run_off == RELAY_RUN_TRUE) &&
			(relay->fsm_run_on != RELAY_RUN_TRUE) )
	{
		relay_pulse_fsm(relay, RELAY_OFF);

		if(relay->fsm_status == RELAY_STATE_STATUS_READY)
		{
			relay->fsm_run_off = RELAY_RUN_FALSE;
		}
	}

	return 0;
}


uint8_t relay_ask_off_pulse_fsm(relay_t *relay)
{
	uint8_t status = 0;
//	if(relay->fsm_run_on != RELAY_RUN_TRUE)
//	{
//		relay->fsm_run_off = RELAY_RUN_TRUE;
//		relay->fsm_init = RELAY_INIT_TRUE;
//		status = 0;
//	}
//	else
//	{
//		status = 1;
//	}

	if(relay->fsm_run_off == RELAY_RUN_FALSE)
	{
		relay->fsm_run_on = RELAY_RUN_FALSE;
		relay->fsm_run_off = RELAY_RUN_TRUE;
		relay->fsm_init = RELAY_INIT_TRUE;
		relay->fsm_state = RELAY_STATE_IDLE;
		status = 0;
	}

	return status;
}
uint8_t relay_ask_on_pulse_fsm(relay_t *relay)
{
	uint8_t status = 0;
//	if(relay->fsm_run_off != RELAY_RUN_TRUE)
//	{
//		relay->fsm_run_on = RELAY_RUN_TRUE;
//		relay->fsm_init = RELAY_INIT_TRUE;
//		status = 0;
//	}
//	else
//	{
//		status = 1;
//	}


	//TODO: (high) Test this change (conditional) intensively
	if(relay->fsm_run_on == RELAY_RUN_FALSE)
	{
		relay->fsm_run_on = RELAY_RUN_TRUE;
		relay->fsm_run_off = RELAY_RUN_FALSE;
		relay->fsm_init = RELAY_INIT_TRUE;
		relay->fsm_state = RELAY_STATE_IDLE;
	}
	return status;
}
uint8_t relay_check_init_fsm(relay_t relay, relay_fsm_init_t *init)
{
	*init = relay.fsm_init;
	return 0;
}

uint8_t relay_acknowledge_init_fsm(relay_t *relay)
{
	relay->fsm_init = RELAY_INIT_FALSE;
	return 0;
}



uint8_t relay_pulse_fsm(relay_t *relay, relay_status_t new_status)
{
	//TODO: (high) check if the implementation of this FSM is necessary

	relay_fsm_state_t state = relay->fsm_state;
//	relay_fsm_init_t fsm_init = relay->fsm_init;
	uint32_t fsm_pulse_counts = relay->fsm_pulse_counts;
	uint32_t fsm_pulse_idx = relay->fsm_pulse_idx;
	relay_fsm_status_t fsm_status = relay->fsm_status;
	relay_fsm_init_t fsm_init = relay->fsm_init;
//	relay_status_t relay_status = relay->relay_status;
//	relay_status_t new_relay_status = relay->new_relay_status;


	switch(state)
	{
		case RELAY_STATE_IDLE:
			fsm_status = RELAY_STATE_STATUS_BUSY;

			if(new_status == RELAY_ON)
			{
				relay_on(relay);
			}
			else
			{
				relay_off(relay);
			}

			if(fsm_pulse_counts == 0)
			{
				state = RELAY_STATE_DEENERGIZING;
			}
			else
			{
				state = RELAY_STATE_WAIT;
			}
			break;
		case RELAY_STATE_WAIT:

			fsm_status = RELAY_STATE_STATUS_BUSY;
			fsm_pulse_idx++;
			if(fsm_pulse_idx > fsm_pulse_counts)
			{
				state = RELAY_STATE_DEENERGIZING;
				fsm_pulse_idx = 0;
			}

			break;
		case RELAY_STATE_DEENERGIZING:
			relay_deenergize(relay);
			fsm_status = RELAY_STATE_STATUS_READY;
			state = RELAY_STATE_IDLE;
			break;
		default:

			break;
	}

	relay->fsm_state = state;
	relay->fsm_init = fsm_init;
	relay->fsm_pulse_idx = fsm_pulse_idx;
	relay->fsm_status = fsm_status;

	return 0;
}

uint8_t relay_on(relay_t *relay)
{
	HAL_GPIO_WritePin(relay->hardware_output_1.port,
						relay->hardware_output_1.pin,
						GPIO_PIN_RESET);

	HAL_GPIO_WritePin(relay->hardware_output_2.port,
						relay->hardware_output_2.pin,
						GPIO_PIN_SET);

	relay->relay_status = RELAY_ON;
	relay->power_status = RELAY_POWER_STATUS_ENERGIZED_ON;
	return 0;
}

uint8_t relay_off(relay_t *relay)
{
	HAL_GPIO_WritePin(relay->hardware_output_1.port,
						relay->hardware_output_1.pin,
						GPIO_PIN_SET);

	HAL_GPIO_WritePin(relay->hardware_output_2.port,
						relay->hardware_output_2.pin,
						GPIO_PIN_RESET);

	relay->relay_status = RELAY_OFF;
	relay->power_status = RELAY_POWER_STATUS_ENERGIZED_OFF;
	return 0;
}


uint8_t relay_deenergize(relay_t *relay)
{
	HAL_GPIO_WritePin(relay->hardware_output_1.port,
						relay->hardware_output_1.pin,
						GPIO_PIN_RESET);

	HAL_GPIO_WritePin(relay->hardware_output_2.port,
						relay->hardware_output_2.pin,
						GPIO_PIN_RESET);

	relay->power_status = RELAY_POWER_STATUS_DEENERGIZED;
	return 0;
}


uint8_t relay_get_status(relay_t relay, relay_status_t *status)
{

	*status = relay.relay_status;
	return 0;
}

uint8_t relay_get_power_status(relay_t relay, relay_power_status_t *status)
{

	*status = relay.power_status;
	return 0;
}




