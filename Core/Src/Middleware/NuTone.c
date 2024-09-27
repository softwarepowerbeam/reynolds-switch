/*
 * NuTone.c
 *
 *  Created on: Aug 30, 2024
 *      Author: Eduardo
 */



#include "Middleware/NuTone.h"

#include "Middleware/output_ctrl.h"



uint8_t nutone_select_state(nutone_t *exhaust_fan);
uint8_t nutone_invert_light_mode(nutone_light_mode_t *mode);

void nutone_setup(nutone_t *exhaust_fan, nutone_relay_handler_t *lights,
											nutone_relay_handler_t *fan,
											deadline_timer_t *ctrl_timer,
											deadline_timer_t *vyv_timeoff)
{

	timer_clock_t deadline;


	exhaust_fan->lights = lights;
	exhaust_fan->fan = fan;

	exhaust_fan->ctrl_timer = ctrl_timer;
	deadline.msec = NUTONE_WHITE_LIGHT_ON_PULSE_TIME;
	deadline_timer_setup(exhaust_fan->ctrl_timer, deadline);
//	exhaust_fan->ctrl_timer->deadline.msec = NUTONE_WHITE_LIGHT_ON_PULSE_TIME;

	exhaust_fan->vyv_timeoff = vyv_timeoff;
	deadline.msec = NUTONE_VYV_TIMEOUT;
	deadline_timer_setup(exhaust_fan->vyv_timeoff, deadline);
//	exhaust_fan->vyv_timeoff->deadline.msec = NUTONE_VYV_TIMEOUT;



	exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_OFF;
	exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_VYV;

	exhaust_fan->light_on_pulses = NUTONE_WHITE_LIGHT_ON_PULSES;
	exhaust_fan->light_on_pulses_idx = 0;

	exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
	exhaust_fan->command = NUTONE_CMD_NONE;

	exhaust_fan->fsm_state = NUTONE_STE_IDLE;


}



uint8_t nutone_fsm(nutone_t *exhaust_fan)
{

//	nutone_states_e fsm_state = exhaust_fan->fsm_state;

	deadline_timer_expired_t is_expired = TIMER_EXPIRED_FALSE;
	deadline_timer_expired_t is_expired_vyv = TIMER_EXPIRED_FALSE;

	deadline_timer_expired_t vyv_timeout_is_expired;


//
//	deadline_timer_check(&exhaust_fan->vyv_timeoff, &vyv_timeout_is_expired);
//
//
//	if(vyv_timeout_is_expired != TIMER_EXPIRED_TRUE)
//	{
//		deadline_timer_set_initial_time(&exhaust_fan->vyv_timeoff);
//		exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_WHITE;
//		exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_VYV;
//	}




	switch(exhaust_fan->fsm_state)
	{
		case NUTONE_STE_IDLE:
			nutone_select_state(exhaust_fan);
			break;
		case NUTONE_STE_FAN_SET_ON:
			relay_ask_on_pulse_fsm(exhaust_fan->fan->relay);
			exhaust_fan->fsm_state = NUTONE_STE_FAN_TURNING_ON;
			break;
		case NUTONE_STE_FAN_TURNING_ON:

			if(exhaust_fan->fan->relay->fsm_run_on == RELAY_RUN_FALSE)
			{
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				exhaust_fan->fsm_state = NUTONE_STE_IDLE;
			}
			break;
		case NUTONE_STE_FAN_SET_OFF:
			relay_ask_off_pulse_fsm(exhaust_fan->fan->relay);
			exhaust_fan->fsm_state = NUTONE_STE_FAN_TURNING_OFF;
			break;
		case NUTONE_STE_FAN_TURNING_OFF:

			if(exhaust_fan->fan->relay->fsm_run_off == RELAY_RUN_FALSE)
			{
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				exhaust_fan->fsm_state = NUTONE_STE_IDLE;
			}

			break;
		case NUTONE_STE_WHITE_LIGHT_SET_ON:

			exhaust_fan->light_on_pulses = NUTONE_WHITE_LIGHT_ON_PULSES;
			exhaust_fan->light_on_pulses_idx = 0;

			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);

			exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_WHITE;
			exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_OFF;

			exhaust_fan->ctrl_timer->deadline.msec = NUTONE_WHITE_LIGHT_ON_PULSE_TIME;
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_ON;
			break;
		case NUTONE_STE_WHITE_LIGHT_ON:
			relay_ask_on_pulse_fsm(exhaust_fan->lights->relay);
			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_TURNING_ON;

			break;
		case NUTONE_STE_WHITE_LIGHT_OFF:
			relay_ask_off_pulse_fsm(exhaust_fan->lights->relay);
			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_TURNING_ON;

			break;
		case NUTONE_STE_WHITE_LIGHT_TURNING_ON:

			deadline_timer_check(exhaust_fan->ctrl_timer, &is_expired);

			if(is_expired != TIMER_EXPIRED_TRUE)
			{
				return 0;//NO state change no variable update needed.
			}

			if(exhaust_fan->lights->relay->relay_status == RELAY_ON)
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->relay->fsm_run_on == RELAY_RUN_FALSE)
				{

					if((exhaust_fan->light_on_pulses_idx) >=
												exhaust_fan->light_on_pulses)
					{
						exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
						exhaust_fan->fsm_state = NUTONE_STE_IDLE;
					}
					else
					{
						exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
					}

					exhaust_fan->light_on_pulses_idx++;
				}
			}
			else
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->relay->fsm_run_off == RELAY_RUN_FALSE)
				{
					exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_ON;
				}
			}

			break;

		case NUTONE_STE_VYV_SET_ON:
			exhaust_fan->light_on_pulses = NUTONE_VYV_ON_PULSES;
			exhaust_fan->light_on_pulses_idx = 0;

			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);

			exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_VYV;
			exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_OFF;

			exhaust_fan->ctrl_timer->deadline.msec = NUTONE_VYV_ON_PULSE_TIME;
			exhaust_fan->fsm_state = NUTONE_STE_VYV_OFF;
			break;

		case NUTONE_STE_VYV_ON:
			relay_ask_on_pulse_fsm(exhaust_fan->lights->relay);
			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);
			exhaust_fan->fsm_state = NUTONE_STE_VYV_TURNING_ON;
			break;

		case NUTONE_STE_VYV_OFF:
			relay_ask_off_pulse_fsm(exhaust_fan->lights->relay);
			deadline_timer_set_initial_time(exhaust_fan->ctrl_timer);
			exhaust_fan->fsm_state = NUTONE_STE_VYV_TURNING_ON;
			break;

		case NUTONE_STE_VYV_TURNING_ON:
			deadline_timer_check(exhaust_fan->ctrl_timer, &is_expired_vyv);

			if(is_expired_vyv != TIMER_EXPIRED_TRUE)
			{
				return 0;//NO state change no variable update needed.
			}

			if(exhaust_fan->lights->relay->relay_status == RELAY_ON)
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->relay->fsm_run_on == RELAY_RUN_FALSE)
				{

					if((exhaust_fan->light_on_pulses_idx) >=
												exhaust_fan->light_on_pulses)
					{
						exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
						exhaust_fan->fsm_state = NUTONE_STE_IDLE;
						exhaust_fan->light_on_pulses_idx = 0;

					}
					else
					{
						exhaust_fan->fsm_state = NUTONE_STE_VYV_OFF;
					}
					exhaust_fan->light_on_pulses_idx++;
				}
			}
			else
			{
				//Waiting for the relay state machine to finish
				if(exhaust_fan->lights->relay->fsm_run_off == RELAY_RUN_FALSE)
				{
					exhaust_fan->fsm_state = NUTONE_STE_VYV_ON;
				}
			}

			break;

		//Second version
		case NUTONE_STE_WHITE_L_SET_ON:
			exhaust_fan->ctrl_timer->deadline.msec =
											NUTONE_WHITE_LIGHT_ON_PULSE_TIME;
			exhaust_fan->light_on_pulses_idx = 0;

			if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_WHITE)
			{
				exhaust_fan->light_on_pulses = 1;
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
			}
			else if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_VYV)
			{
				exhaust_fan->light_on_pulses = 0;
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
			}
			else
			{

				if(exhaust_fan->next_light_mode == NUTONE_LIGHT_MODE_VYV)
				{
					exhaust_fan->light_on_pulses = 1;
				}
				else if(exhaust_fan->next_light_mode == NUTONE_LIGHT_MODE_WHITE)
				{
					exhaust_fan->light_on_pulses = 0;
				}
				else
				{
					//Do nothing (not possible if the system works
				}
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_ON;

			}
			//The order of this matters
			exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_WHITE;
			exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_OFF;
			break;

		case NUTONE_STE_VYV_L_SET_ON:
			exhaust_fan->ctrl_timer->deadline.msec = NUTONE_VYV_ON_PULSE_TIME;
			exhaust_fan->light_on_pulses_idx = 0;

			if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_WHITE)
			{
				exhaust_fan->light_on_pulses = 0;
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
			}
			else if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_VYV)
			{
				exhaust_fan->light_on_pulses = 1;
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_OFF;
			}
			else
			{

				if(exhaust_fan->next_light_mode == NUTONE_LIGHT_MODE_VYV)
				{
					exhaust_fan->light_on_pulses = 0;
				}
				else if(exhaust_fan->next_light_mode == NUTONE_LIGHT_MODE_WHITE)
				{
					exhaust_fan->light_on_pulses = 1;
				}
				else
				{
					//Do nothing (not possible if the system works
				}
				exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_ON;

			}
			//The order of this matters
			exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_VYV;
			exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_OFF;
			break;


		//White light and VYV just need to turn-off the relay to be deactivated
		case NUTONE_STE_LIGHTS_OFF:
			relay_ask_off_pulse_fsm(exhaust_fan->lights->relay);
			exhaust_fan->fsm_state = NUTONE_STE_LIGHTS_TURNING_OFF;
			break;
		case NUTONE_STE_LIGHTS_TURNING_OFF:
			if(exhaust_fan->lights->relay->fsm_run_off == RELAY_RUN_FALSE)
			{
				deadline_timer_set_initial_time(exhaust_fan->vyv_timeoff);
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
				exhaust_fan->fsm_state = NUTONE_STE_LIGHTS_TURNED_OFF;
			}
			break;

		case NUTONE_STE_LIGHTS_TURNED_OFF:



			if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_VYV)
			{
				exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_OFF;
				exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_WHITE;
				exhaust_fan->fsm_state = NUTONE_STE_IDLE;
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
			}
			else if(exhaust_fan->current_light_mode == NUTONE_LIGHT_MODE_WHITE)
			{
				exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_OFF;
				exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_VYV;
				exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;

			}
			else
			{
				nutone_select_state(exhaust_fan);

				if(exhaust_fan->next_light_mode == NUTONE_LIGHT_MODE_VYV)
				{

					deadline_timer_check(exhaust_fan->vyv_timeoff,
														&vyv_timeout_is_expired);


					if( (vyv_timeout_is_expired == TIMER_EXPIRED_TRUE) &&
						(exhaust_fan->cmd_state == NUTONE_CMD_STE_READY))
					{
						//deadline_timer_set_initial_time(exhaust_fan->vyv_timeoff);
						exhaust_fan->current_light_mode = NUTONE_LIGHT_MODE_OFF;
						exhaust_fan->next_light_mode = NUTONE_LIGHT_MODE_WHITE;
						exhaust_fan->fsm_state = NUTONE_STE_IDLE;
						exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;

					}
				}
				else
				{
					exhaust_fan->cmd_state = NUTONE_CMD_STE_READY;
					exhaust_fan->fsm_state = NUTONE_STE_IDLE;
				}
			}
			break;
		default:
			exhaust_fan->fsm_state = NUTONE_STE_IDLE;
			break;
	}


//	exhaust_fan->fsm_state = fsm_state;
	return 0;

}

uint8_t nutone_set_command(nutone_t *exhaust_fan, nutone_command_e command)
{
	exhaust_fan->command = command;
	return 0;
}

uint8_t nutone_get_cmd_status(nutone_t exhaust_fan,
												nutone_cmd_state_t *cmd_status)
{
	*cmd_status = exhaust_fan.cmd_state;
	return 0;
}


//#define NO_MEMORY

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
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_FAN_TURN_OFF:
			exhaust_fan->fsm_state = NUTONE_STE_FAN_SET_OFF;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_WHITE_TURN_ON:
#ifdef 	NO_MEMORY
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_LIGHT_SET_ON;
#else	//NO_MEMORY
			exhaust_fan->fsm_state = NUTONE_STE_WHITE_L_SET_ON;
#endif	//NO_MEMORY
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_WHITE_TURN_OFF:
			exhaust_fan->fsm_state = NUTONE_STE_LIGHTS_OFF;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_VYV_TURN_ON:
#ifdef 	NO_MEMORY
			exhaust_fan->fsm_state = NUTONE_STE_VYV_SET_ON;
#else	//NO_MEMORY
			exhaust_fan->fsm_state = NUTONE_STE_VYV_L_SET_ON;
#endif	//NO_MEMORY
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_VYV_TURN_OFF:
			exhaust_fan->fsm_state = NUTONE_STE_LIGHTS_OFF;
			exhaust_fan->cmd_state = NUTONE_CMD_STE_BUSY;
			exhaust_fan->command = NUTONE_CMD_NONE;
			break;
		case NUTONE_CMD_MANUAL_SURFACESHIELD_SET:
			break;
		case NUTONE_CMD_MANUAL_SURFACESHIELD_CLEAR:
			break;
		case NUTONE_CMD_AUTOMATIC_SURFACESHIELD_SET:
			break;
		case NUTONE_CMD_AUTOMATIC_SURFACESHIELD_CLEAR:
			break;
		case NUTONE_CMD_WHITE_LIGHT_SET:
			break;
		case NUTONE_CMD_WHITE_LIGHT_CLEAR:
			break;
		default:
			break;
	}


	return 0;
}


uint8_t nutone_invert_light_mode(nutone_light_mode_t *mode)
{
	if(*mode == NUTONE_LIGHT_MODE_VYV)
	{
		*mode = NUTONE_LIGHT_MODE_WHITE;
	}
	else if(*mode == NUTONE_LIGHT_MODE_WHITE)
	{
		*mode = NUTONE_LIGHT_MODE_VYV;
	}
	else
	{
		//do nothing
	}
	return 0;
}

uint8_t nutone_set_mode(nutone_t *exhaust_fan, nutone_mode_e op_mode)
{
	exhaust_fan->op_mode = op_mode;
	return 0;
}







