/*
 * output_ctrl.cpp
 *
 *  Created on: Aug 29, 2024
 *      Author: Eduardo
 */



#include "Middleware/output_ctrl.h"






void output_fsm_ctrl(relay_t *actuator, deadline_timer_t *deadline_timer)
{
	relay_fsm_init_t init = RELAY_INIT_FALSE;
	deadline_timer_expired_t expired;

	if(actuator->fsm_run_on == RELAY_RUN_TRUE)
	{
		relay_check_init_fsm(*actuator, &init);

		if(init == RELAY_INIT_TRUE)
		{
			deadline_timer_set_initial_time(deadline_timer);
			relay_acknowledge_init_fsm(actuator);
		}

		deadline_timer_check(deadline_timer, &expired);

		if(expired == TIMER_EXPIRED_TRUE)
		{
			relay_on_pulse_fsm(actuator);
			deadline_timer_set_initial_time(deadline_timer);
		}
	}

	if(actuator->fsm_run_off == RELAY_RUN_TRUE)
	{
	  relay_check_init_fsm(*actuator, &init);

	  if(init == RELAY_INIT_TRUE)
	  {
		  deadline_timer_set_initial_time(deadline_timer);
		  relay_acknowledge_init_fsm(actuator);
	  }

	  deadline_timer_check(deadline_timer, &expired);

	  if(expired == TIMER_EXPIRED_TRUE)
	  {
		  relay_off_pulse_fsm(actuator);
		  deadline_timer_set_initial_time(deadline_timer);
	  }
	}
}


void output_led_indicator(led_signal_t *led_signal,
								deadline_timer_t *deadline_timer)
{
	deadline_timer_expired_t indicator_timer_expired;
	deadline_timer_check(deadline_timer, &indicator_timer_expired);

	if(indicator_timer_expired == TIMER_EXPIRED_TRUE)
	{
		led_signal_fsm(led_signal);
		deadline_timer_set_initial_time(deadline_timer);
	}
}



