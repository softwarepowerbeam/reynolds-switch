/*
 * NuTone.c
 *
 *  Created on: Aug 30, 2024
 *      Author: Eduardo
 */



#include "Middleware/NuTone.h"

#include "Middleware/output_ctrl.h"



uint8_t nutone_select_state(nutone_t *exhaust_fan);


void nutone_setup(nutone_t *exhaust_fan, relay_t *lights, relay_t *fan,
											deadline_timer_t *deadline_timer)
{
	exhaust_fan->lights = lights;
	exhaust_fan->deadline_timer = deadline_timer;
	exhaust_fan->white_light_attempts = 0;
	exhaust_fan->deadline_timer->deadline = NUTONE_WHITE_LIGHT_ON_PULSE_TIME;
}



uint8_t nutone_fsm(nutone_t *exhaust_fan)
{

	relay_t *actuator = exhaust_fan->actuator;
	deadline_timer_t *deadline_timer = exhaust_fan->deadline_timer;
	nutone_states_e fsm_state = exhaust_fan->fsm_state;
	nutone_mode_e op_mode = exhaust_fan->op_mode;



	switch(fsm_state)
	{
		case NUTONE_STE_IDLE:
			nutone_select_state(exhaust_fan);
			break;
		case NUTONE_STE_FAN_SET_ON:
			exhaust_fan->fan->fsm_run_on = RELAY_RUN_TRUE;
			fsm_state = NUTONE_STE_FAN_TURNING_ON;
			break;
		case NUTONE_STE_FAN_TURNING_ON:

			if(exhaust_fan->fan->fsm_run_on == RELAY_RUN_FALSE)
			{
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				fsm_state = NUTONE_STE_IDLE;
			}
		case NUTONE_STE_FAN_SET_OFF:
			exhaust_fan->fan->fsm_run_off = RELAY_RUN_TRUE;
			fsm_state = NUTONE_STE_FAN_TURNING_OFF;
			break;
		case NUTONE_STE_FAN_TURNING_OFF:

			if(exhaust_fan->fan->fsm_run_off == RELAY_RUN_FALSE)
			{
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				fsm_state = NUTONE_STE_IDLE;
			}

			break;
		case NUTONE_STE_WHITE_LIGHT_SET_ON:
			exhaust_fan->lights->fsm_run_on = RELAY_RUN_TRUE;
			exhaust_fan->deadline_timer->deadline = NUTONE_WHITE_LIGHT_ON_PULSE_TIME;
			fsm_state = UTONE_STE_WHITE_LIGHT_ON;
			break;
		case NUTONE_STE_WHITE_LIGHT_ON:
			exhaust_fan->lights->fsm_run_on = RELAY_RUN_TRUE;
			fsm_state = UTONE_STE_WHITE_LIGHT_ON;
			break;
		case NUTONE_STE_WHITE_LIGHT_OFF:
			exhaust_fan->lights->fsm_run_off = RELAY_RUN_TRUE;
			fsm_state = NUTONE_STE_WHITE_LIGHT_TURNING_ON;

			break;
		case NUTONE_STE_WHITE_LIGHT_TURNING_ON:

			deadline_timer_expired_t is_expired = TIMER_EXPIRED_FALSE;

			deadline_timer_check(&exhaust_fan->deadline_timer, &is_expired);

			if(is_expired != TIMER_EXPIRED_TRUE)
			{
				return 0;//NO state change no variable update.
			}


			if(exhaust_fan->lights->relay_status == RELAY_ON)
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->fsm_run_on == RELAY_RUN_FALSE)
				{
					exhaust_fan->white_light_pulses++;

					if((exhaust_fan->white_light_pulses * 2) >=
							NUTONE_WHITE_LIGHT_ON_ATTEMPTS)
					{
						exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
						fsm_state = NUTONE_STE_IDLE;
						white_light_attempts = 0;
					}
					else
					{
						fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
					}
				}
			}
			else
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->fsm_run_off == RELAY_RUN_FALSE)
				{
					fsm_state = NUTONE_STE_WHITE_LIGHT_ON;
				}
			}


			break;

		case NUTONE_STE_LIGHTS_OFF:
			exhaust_fan->lights->fsm_run_off = RELAY_RUN_TRUE;
			fsm_state = NUTONE_STE_FAN_TURNING_ON;
			break;
		case NUTONE_STE_LIGHTS_TURNING_OFF:
			if(exhaust_fan->lights->fsm_run_off == RELAY_RUN_FALSE)
			{
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				fsm_state = NUTONE_STE_IDLE;
			}
			break;
	}


	exhaust_fan->fsm_state = fsm_state;
	return 0;

}

uint8_t nutone_set_command(nutone_t *exhaust_fan, nutone_command_e command)
{
	exhaust_fan->command = command;
}



uint8_t nutone_select_state(nutone_t *exhaust_fan)
{
	if(exhaust_fan->cmd_state != NUTONE_CMD_STE_READY)
	{
		return 1;
	}
	switch(exhaust_fan->command)
	{
		case NUTONE_CMD_FAN_TURN_ON:
			exhaust_fan->fsm_state = NUTONE_STE_FAN_SET_ON;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			break;
		case NUTONE_CMD_FAN_TURN_OFF:
			exhaust_fan->fsm_state = NUTONE_STE_FAN_SET_OFF;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			break;
		case NUTONE_CMD_WHITE_TURN_ON:
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_SET_ON;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			break;
		case NUTONE_CMD_WHITE_TURN_OFF:
			exhaust_fan->fsm_state = NUTONE_STE_LIGHTS_OFF;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			break;
	}

	return 0;
}


uint8_t nutone_set_mode(nutone_t *exhaust_fan, nutone_mode_e op_mode)
{
	exhaust_fan->op_mode = op_mode;
}







