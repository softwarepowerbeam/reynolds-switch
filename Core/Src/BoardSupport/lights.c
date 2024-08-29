/*
 * lights.c
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */

#include "BoardSupport/lights.h"


uint8_t light_setup(light_t *light, light_gpio_t hardware_output_1,
												light_gpio_t hardware_output_2)
{

	light->hardware_output_1 = hardware_output_1;
	light->hardware_output_2 = hardware_output_2;
	light_deenergize(light);
	light->light_status = LIGHT_OFF;
	light->fsm_run_on = LIGHT_RUN_FALSE;
	light->fsm_run_off = LIGHT_RUN_FALSE;
	light->fsm_pulse_counts = 1;
	return 0;

}
uint8_t light_on_pulse_fsm(light_t *light)
{
	if( (light->fsm_run_on == LIGHT_RUN_TRUE) &&
		(light->fsm_run_off != LIGHT_RUN_TRUE) )
	{
		light_pulse_fsm(light, LIGHT_ON);

		if(light->fsm_status == LIGHT_STATE_STATUS_READY)
		{
			light->fsm_run_on = LIGHT_RUN_FALSE;
		}
	}
	return 0;
}

uint8_t light_off_pulse_fsm(light_t *light)
{
	if( (light->fsm_run_off == LIGHT_RUN_TRUE) &&
			(light->fsm_run_on != LIGHT_RUN_TRUE) )
	{
		light_pulse_fsm(light, LIGHT_OFF);

		if(light->fsm_status == LIGHT_STATE_STATUS_READY)
		{
			light->fsm_run_off = LIGHT_RUN_FALSE;
		}
	}

	return 0;
}


uint8_t light_ask_off_pulse_fsm(light_t *light)
{
	uint8_t status = 0;
//	if(light->fsm_run_on != LIGHT_RUN_TRUE)
//	{
//		light->fsm_run_off = LIGHT_RUN_TRUE;
//		light->fsm_init = LIGHT_INIT_TRUE;
//		status = 0;
//	}
//	else
//	{
//		status = 1;
//	}

	light->fsm_run_on = LIGHT_RUN_FALSE;
	light->fsm_run_off = LIGHT_RUN_TRUE;
	light->fsm_init = LIGHT_INIT_TRUE;
	light->fsm_state = LIGHT_STATE_IDLE;
	status = 0;

	return status;
}
uint8_t light_ask_on_pulse_fsm(light_t *light)
{
	uint8_t status = 0;
//	if(light->fsm_run_off != LIGHT_RUN_TRUE)
//	{
//		light->fsm_run_on = LIGHT_RUN_TRUE;
//		light->fsm_init = LIGHT_INIT_TRUE;
//		status = 0;
//	}
//	else
//	{
//		status = 1;
//	}

	light->fsm_run_on = LIGHT_RUN_TRUE;
	light->fsm_run_off = LIGHT_RUN_FALSE;
	light->fsm_init = LIGHT_INIT_TRUE;
	light->fsm_state = LIGHT_STATE_IDLE;

	return status;
}
uint8_t light_check_init_fsm(light_t light, light_fsm_init_t *init)
{
	*init = light.fsm_init;
	return 0;
}

uint8_t light_acknowledge_init_fsm(light_t *light)
{
	light->fsm_init = LIGHT_INIT_FALSE;
	return 0;
}



uint8_t light_pulse_fsm(light_t *light, light_status_t new_status)
{
	//TODO: (high) check if the implementation of this FSM is necessary

	light_fsm_state_t state = light->fsm_state;
//	light_fsm_init_t fsm_init = light->fsm_init;
	uint32_t fsm_pulse_counts = light->fsm_pulse_counts;
	uint32_t fsm_pulse_idx = light->fsm_pulse_idx;
	light_fsm_status_t fsm_status = light->fsm_status;
	light_fsm_init_t fsm_init = light->fsm_init;
//	light_status_t light_status = light->light_status;
//	light_status_t new_light_status = light->new_light_status;


	switch(state)
	{
		case LIGHT_STATE_IDLE:
//			if(fsm_init == LIGHT_INIT_TRUE)
//			{
				fsm_status = LIGHT_STATE_STATUS_BUSY;

				if(new_status == LIGHT_ON)
				{
					light_on(light);
				}
				else
				{
					light_off(light);
				}

				if(fsm_pulse_counts == 0)
				{
					state = LIGHT_STATE_DEENERGIZING;
				}
				else
				{
					state = LIGHT_STATE_WAIT;
				}
//				fsm_init = LIGHT_INIT_FALSE;
//			}
			break;
		case LIGHT_STATE_WAIT:

			fsm_status = LIGHT_STATE_STATUS_BUSY;
			fsm_pulse_idx++;
			if(fsm_pulse_idx > fsm_pulse_counts)
			{
				state = LIGHT_STATE_DEENERGIZING;
				fsm_pulse_idx = 0;
			}

			break;
		case LIGHT_STATE_DEENERGIZING:
			light_deenergize(light);
			fsm_status = LIGHT_STATE_STATUS_READY;
			state = LIGHT_STATE_IDLE;
			break;
		default:

			break;
	}

	light->fsm_state = state;
	light->fsm_init = fsm_init;
	light->fsm_pulse_idx = fsm_pulse_idx;
	light->fsm_status = fsm_status;

	return 0;
}

uint8_t light_on(light_t *light)
{
	HAL_GPIO_WritePin(light->hardware_output_1.port,
						light->hardware_output_1.pin,
						GPIO_PIN_RESET);

	HAL_GPIO_WritePin(light->hardware_output_2.port,
						light->hardware_output_2.pin,
						GPIO_PIN_SET);

	light->light_status = LIGHT_ON;
	light->power_status = LIGHT_POWER_STATUS_ENERGIZED_ON;
	return 0;
}

uint8_t light_off(light_t *light)
{
	HAL_GPIO_WritePin(light->hardware_output_1.port,
						light->hardware_output_1.pin,
						GPIO_PIN_SET);

	HAL_GPIO_WritePin(light->hardware_output_2.port,
						light->hardware_output_2.pin,
						GPIO_PIN_RESET);

	light->light_status = LIGHT_OFF;
	light->power_status = LIGHT_POWER_STATUS_ENERGIZED_OFF;
	return 0;
}


uint8_t light_deenergize(light_t *light)
{
	HAL_GPIO_WritePin(light->hardware_output_1.port,
						light->hardware_output_1.pin,
						GPIO_PIN_RESET);

	HAL_GPIO_WritePin(light->hardware_output_2.port,
						light->hardware_output_2.pin,
						GPIO_PIN_RESET);

	light->power_status = LIGHT_POWER_STATUS_DEENERGIZED;
	return 0;
}


uint8_t light_get_status(light_t light, light_status_t *status)
{

	*status = light.light_status;
	return 0;
}

uint8_t light_get_power_status(light_t light, light_power_status_t *status)
{

	*status = light.power_status;
	return 0;
}




