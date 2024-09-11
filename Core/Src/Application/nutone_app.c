/*
 * nutone_app.c
 *
 *  Created on: Sep 4, 2024
 *      Author: Eduardo
 */

#include "Application/nutone_app.h"

//TODO: Change this to a different, more general, library
void nutone_app_check_button_event(deadline_timer_t *deadline_events, button_t *button);
void nutone_app_set_outputs(nutone_app_t *nutone_app_hand);
uint8_t nutone_white_fsm(nutone_app_t *nutone_app_hand);
uint8_t nutone_vyv_fsm(nutone_app_t *nutone_app_hand);
uint8_t nutone_fan_fsm(nutone_app_t *nutone_app_hand);

uint8_t nutone_app_config(nutone_app_t *nutone_app_hand, nutone_t *nutone_dev,
							motion_handler_t *motion_hand,
							button_handler_t *button_fan,
							button_handler_t *button_white,
							button_handler_t *button_vyv,
							led_signal_handler_t *led_signal_hand,
							deadline_timer_t *timer_fsm_fan,
							deadline_timer_t *timer_fsm_white,
							deadline_timer_t *timer_fsm_vyv,
							deadline_timer_t *timer_fsm_vyv_timeout,
							deadline_timer_t *timer_fsm_vyv_safe)
{
	nutone_app_hand->motion_hand = motion_hand;
	nutone_app_hand->nutone_dev = nutone_dev;
	nutone_app_hand->button_white = button_white;
	nutone_app_hand->button_fan = button_fan;
	nutone_app_hand->button_vyv = button_vyv;
	nutone_app_hand->led_signal_hand = led_signal_hand;
	led_signal_start(nutone_app_hand->led_signal_hand->led_signal);
	nutone_app_hand->timer_fsm_white = timer_fsm_white;
	nutone_app_hand->timer_fsm_vyv = timer_fsm_vyv;
	nutone_app_hand->timer_fsm_vyv_safe = timer_fsm_vyv_safe;
	nutone_app_hand->timer_fsm_vyv_timeout = timer_fsm_vyv_timeout;
	nutone_app_hand->timer_fsm_fan = timer_fsm_fan;


	nutone_app_hand->signals.uv_abort = MOTION_ABORT_FALSE;
	nutone_app_hand->signals.wait = MOTION_UV_WAIT_FALSE;
	nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
	nutone_app_hand->signals.motion_light = MOTION_ISR_ATTENDED;
	return 0;
}

uint8_t nutone_app_process(nutone_app_t *nutone_app_hand)
{

	nutone_app_check_events(nutone_app_hand);

	nutone_app_fsm(nutone_app_hand);

	nutone_app_set_outputs(nutone_app_hand);

	return 0;
}



uint8_t nutone_app_fsm(nutone_app_t *nutone_app_hand)
{

//	led_signal_start(nutone_app_hand->led_signal_hand->led_signal);
//	nutone_app_hand->led_signal_hand->led_signal->type = LED_SIGNAL_BLINK;

//	relay_ask_on_pulse_fsm(nutone_app_hand->nutone_dev->lights->relay);

	nutone_fan_fsm(nutone_app_hand);

	nutone_white_fsm(nutone_app_hand);

	nutone_vyv_fsm(nutone_app_hand);

	nutone_fsm(nutone_app_hand->nutone_dev);


	return 0;

}


uint8_t nutone_fan_fsm(nutone_app_t *nutone_app_hand)
{
	if(nutone_app_hand->button_fan->button->edge_attended ==
														BUTTON_ISR_UNATTENDED)
	{
		if(nutone_app_hand->button_fan->button->edge == BUTTON_EDGE_POSITIVE)
		{
			nutone_set_command(nutone_app_hand->nutone_dev,
														NUTONE_CMD_FAN_TURN_ON);
		}
		else
		{
			nutone_set_command(nutone_app_hand->nutone_dev,
													NUTONE_CMD_FAN_TURN_OFF);
		}
		nutone_app_hand->button_fan->button->edge_attended = BUTTON_ISR_ATTENDED;
	}
	return 0;
}

uint8_t nutone_white_fsm(nutone_app_t *nutone_app_hand)
{

	button_isr_status_t button_isr_status;
	button_edge_t edge;

	button_check_isr_request(*nutone_app_hand->button_white->button,
													&button_isr_status, &edge);


	if( (button_isr_status == BUTTON_ISR_UNATTENDED) ||
			(nutone_app_hand->signals.motion_light == MOTION_ISR_UNATTENDED))
	{
		if(button_isr_status == BUTTON_ISR_UNATTENDED)
		{
			button_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;

			if(edge == BUTTON_EDGE_NEGATIVE)
			{
				nutone_app_hand->fsm_state_white = MOTION_LIGHT_TURN_OFF_LIGHT;
			}
			else
			{
				nutone_app_hand->fsm_state_white = MOTION_LIGHT_CHECK_BUTTON;
			}
		}

		nutone_app_hand->button_white->button->edge_attended = button_isr_status;

		if( (nutone_app_hand->signals.motion_light == MOTION_ISR_UNATTENDED) &&
			(nutone_app_hand->fsm_state_white == MOTION_LIGHT_IDLE))
		{
			nutone_app_hand->fsm_state_white = MOTION_LIGHT_CHECK_BUTTON;
			nutone_app_hand->signals.motion_light  = MOTION_ISR_ATTENDED;
		}

	}



	switch(nutone_app_hand->fsm_state_white)
	{
		case MOTION_LIGHT_IDLE:

			__NOP();//Do nothing

			break;
		case MOTION_LIGHT_CHECK_BUTTON:

			button_status_t button_status = BUTTON_OFF;
			button_get_status(nutone_app_hand->button_white->button,
															&button_status);

			if(button_status == BUTTON_ON)
			{
				nutone_app_hand->fsm_state_white  = MOTION_LIGHT_TURN_ON_LIGHT;
				nutone_app_hand->signals.motion_light = MOTION_ISR_ATTENDED;
			}
			else
			{
				 //if an edge where detected an error occurred in the button
				nutone_app_hand->fsm_state_white = MOTION_LIGHT_TURN_OFF_LIGHT;
				nutone_app_hand->signals.motion_light = MOTION_ISR_ATTENDED;
			}

			break;
		case MOTION_LIGHT_TURN_ON_LIGHT:

			//this is done in another fsm
			nutone_set_command(nutone_app_hand->nutone_dev,
													NUTONE_CMD_WHITE_TURN_ON);
			nutone_app_hand->fsm_state_white = MOTION_LIGHT_INIT_TIMER;

			break;
		case MOTION_LIGHT_INIT_TIMER:
			//This updates the timer count
			deadline_timer_set_initial_time(nutone_app_hand->timer_fsm_white);
			nutone_app_hand->fsm_state_white  = MOTION_LIGHT_WAIT_EXPIRATION;

			break;
		case MOTION_LIGHT_WAIT_EXPIRATION:

			deadline_timer_expired_t deadline_expired;

			deadline_timer_check(nutone_app_hand->timer_fsm_white,
												&deadline_expired);

			if(deadline_expired == TIMER_EXPIRED_TRUE)
			{
				nutone_app_hand->fsm_state_white = MOTION_LIGHT_TURN_OFF_LIGHT;
			}

			if(nutone_app_hand->signals.motion_light == MOTION_ISR_UNATTENDED)
			{
				nutone_app_hand->signals.motion_light  = MOTION_ISR_ATTENDED;
				nutone_app_hand->fsm_state_white = MOTION_LIGHT_INIT_TIMER;
			}
			break;
		case MOTION_LIGHT_TURN_OFF_LIGHT:

			deadline_timer_force_expiration(nutone_app_hand->timer_fsm_white);
			//this is done in another fsm
			nutone_set_command(nutone_app_hand->nutone_dev,
													NUTONE_CMD_WHITE_TURN_OFF);
			nutone_app_hand->fsm_state_white = MOTION_LIGHT_IDLE;
			break;
		default:
			nutone_app_hand->fsm_state_white = MOTION_LIGHT_IDLE;
			break;
	}


	return 0;



}


uint8_t nutone_vyv_fsm(nutone_app_t *nutone_app_hand)
{

	button_isr_status_t button_isr_status;
	button_edge_t edge;
	pyd1598_motion_isr_status_t motion_isr_status;
	deadline_timer_expired_t deadline_safe_expired;
	deadline_timer_expired_t deadline_timeout_expired;

	button_check_isr_request(*nutone_app_hand->button_vyv->button, &button_isr_status, &edge);
	pyd1598_check_isr_request(*nutone_app_hand->motion_hand->motion_sensor, &motion_isr_status);


	if(button_isr_status == BUTTON_ISR_UNATTENDED)
	{

		//Change if abort is not required with a second push
		if(nutone_app_hand->button_vyv->button->push_status == BUTTON_PUSH_ON)
		{
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_INIT_TIMEOUT_TIMER;
		}
		else
		{
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_ABORT;
		}

		button_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;
	}
	nutone_app_hand->button_vyv->button->edge_attended = button_isr_status;


	if(nutone_app_hand->signals.uv_abort == MOTION_ABORT_TRUE)
	{
		nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_ABORT;
		nutone_app_hand->signals.uv_abort = MOTION_ABORT_FALSE;
	}

	if(	nutone_app_hand->nutone_dev->current_light_mode ==
														NUTONE_LIGHT_MODE_WHITE)
	{
		nutone_app_hand->signals.wait = MOTION_UV_WAIT_TRUE;
	}
	else
	{
		nutone_app_hand->signals.wait = MOTION_UV_WAIT_FALSE;
	}


	switch(nutone_app_hand->fsm_state_uyu)
	{
		case MOTION_LIGHT_UV_IDLE:


			if(nutone_app_hand->signals.motion_uv  == MOTION_ISR_UNATTENDED)
			{
				nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
			}

			__NOP();//Do nothing

			break;
		case MOTION_LIGHT_UV_INIT_TIMEOUT_TIMER:
			//Initialize deadline_timeout
			deadline_timer_set_initial_time(nutone_app_hand->timer_fsm_vyv_timeout);

			//Start LED indicator
			led_signal_start(nutone_app_hand->led_signal_hand->led_signal);
			nutone_app_hand->led_signal_hand->led_signal->type = LED_SIGNAL_BLINK;

			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT;

			break;
		case MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT:

			if(nutone_app_hand->signals.motion_uv  == MOTION_ISR_UNATTENDED)
			{
				nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
			}
			deadline_timer_check(nutone_app_hand->timer_fsm_vyv_timeout,
													&deadline_timeout_expired);

			if(deadline_timeout_expired == TIMER_EXPIRED_TRUE)//This should be a long timer
			{
				nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
				nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_INIT_SAFE_TIMER;
				nutone_app_hand->led_signal_hand->led_signal->type = LED_SIGNAL_SOLID;
			}
			break;

		case MOTION_LIGHT_UV_INIT_SAFE_TIMER:
			//TODO: (high) add a timeout
			deadline_timer_set_initial_time(nutone_app_hand->timer_fsm_vyv_safe);

			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_WAIT_SAFE_TIMER;
			break;

		case MOTION_LIGHT_UV_WAIT_SAFE_TIMER:

			deadline_timer_check(nutone_app_hand->timer_fsm_vyv_safe,
														&deadline_safe_expired);

			if(deadline_safe_expired == TIMER_EXPIRED_TRUE)//This should be a long timer
			{
				nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_TURN_ON_LIGHT;
			}
			else
			{
				if(nutone_app_hand->signals.motion_uv== MOTION_ISR_UNATTENDED)
				{
					nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
					nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_ABORT;
				}
				else
				{
					if(nutone_app_hand->signals.wait == MOTION_UV_WAIT_TRUE)
					{
						nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_INIT_SAFE_TIMER;
					}
				}

			}

			break;
		case MOTION_LIGHT_UV_TURN_ON_LIGHT:
			//this is done in another fsm
//			relay_ask_on_pulse_fsm(light_uv);
			nutone_set_command(nutone_app_hand->nutone_dev,
														NUTONE_CMD_VYV_TURN_ON);
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_INIT_TIMER;

			break;
		case MOTION_LIGHT_UV_INIT_TIMER:
			deadline_timer_set_initial_time(nutone_app_hand->timer_fsm_vyv);
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_WAIT_EXPIRATION;
			break;
		case MOTION_LIGHT_UV_WAIT_EXPIRATION:
			deadline_timer_expired_t deadline_expired;
			deadline_timer_check(nutone_app_hand->timer_fsm_vyv,
															&deadline_expired);

			if(deadline_expired == TIMER_EXPIRED_TRUE)
			{
				nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_TURN_OFF_LIGHT;
			}
			else
			{
				//Do nothing
			}

			if(nutone_app_hand->signals.motion_uv == MOTION_ISR_UNATTENDED)
			{
				nutone_app_hand->signals.motion_uv = MOTION_ISR_ATTENDED;
				nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_ABORT;
			}

			break;
		case MOTION_LIGHT_UV_ABORT:
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_TURN_OFF_LIGHT;

			break;
		case MOTION_LIGHT_UV_TURN_OFF_LIGHT:
			//this is done in another fsm
			deadline_timer_force_expiration(nutone_app_hand->timer_fsm_vyv);
			deadline_timer_force_expiration(nutone_app_hand->timer_fsm_vyv_timeout);
			deadline_timer_force_expiration(nutone_app_hand->timer_fsm_vyv_safe);
			nutone_app_hand->button_vyv->button->push_status = BUTTON_PUSH_OFF;

			//Send signal to turn off UV lamp

			nutone_set_command(nutone_app_hand->nutone_dev,
														NUTONE_CMD_VYV_TURN_OFF);
			//Send signal stop LED indicator
			led_signal_stop(nutone_app_hand->led_signal_hand->led_signal);
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_IDLE;

			break;
		default:
			nutone_app_hand->fsm_state_uyu = MOTION_LIGHT_UV_IDLE;
			break;
	}




	return 0;
}

void nutone_app_check_events(nutone_app_t *nutone_app_hand)
{
	//variables to check motion events
	pyd1598_motion_isr_status_t motion_isr_status;
	//To check UV button state:
	button_isr_status_t button_isr_stat;
	button_edge_t check_edge;


	pyd1598_read_wakeup_signal(nutone_app_hand->motion_hand->motion_sensor,
															&motion_isr_status);

	if(motion_isr_status == PYD1598_MOTION_ISR_UNATTENDED)
	{
		nutone_app_hand->signals.motion_light = MOTION_ISR_UNATTENDED;
		nutone_app_hand->signals.motion_uv = MOTION_ISR_UNATTENDED;
		nutone_app_hand->motion_hand->motion_sensor->motion_sensed = PYD1598_MOTION_ISR_ATTENDED;
	}

	nutone_app_check_button_event(nutone_app_hand->button_white->btn_timer,
								nutone_app_hand->button_white->button);
	nutone_app_check_button_event(nutone_app_hand->button_vyv->btn_timer,
									nutone_app_hand->button_vyv->button);
	nutone_app_check_button_event(nutone_app_hand->button_fan->btn_timer,
									nutone_app_hand->button_fan->button);


	//Since a push button does not maintain its state, memory is needed.
	button_check_isr_request(*nutone_app_hand->button_vyv->button,
												&button_isr_stat, &check_edge);
	if(button_isr_stat == BUTTON_ISR_UNATTENDED)
	{
		if(nutone_app_hand->button_vyv->button->push_status != BUTTON_PUSH_ON)
		{
			nutone_app_hand->button_vyv->button->push_status = BUTTON_PUSH_ON;
		}
		else
		{
			nutone_app_hand->button_vyv->button->push_status = BUTTON_PUSH_OFF;
		}
	}



}

void nutone_app_check_button_event(deadline_timer_t *deadline_events, button_t *button)
{
	deadline_timer_expired_t timer_expired;
	if(button->debounce_lock == BUTTON_DEBOUNCE_LOCK_ON)
	{
		//TODO: (medium) Check if a new timer should be configured
		deadline_timer_check(deadline_events, &timer_expired);

		if(timer_expired == TIMER_EXPIRED_TRUE)
		{
			button_debounce_fsm(button);
			deadline_timer_set_initial_time(deadline_events);
		}
	}
}


void nutone_app_set_outputs(nutone_app_t *nutone_app_hand)
{
	output_fsm_ctrl(nutone_app_hand->nutone_dev->lights->relay,
					nutone_app_hand->nutone_dev->lights->deadline_timer);

	output_fsm_ctrl(nutone_app_hand->nutone_dev->fan->relay,
					nutone_app_hand->nutone_dev->fan->deadline_timer);

	output_led_indicator(nutone_app_hand->led_signal_hand->led_signal,
							nutone_app_hand->led_signal_hand->led_signal_timer);

}



