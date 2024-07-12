/*
 * timer_alarm.c
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */
#include "tim.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "deadline_timer.h"

//TODO: (High) fix this to include seconds
uint8_t deadline_timer_setup(deadline_timer_t *deadline_timer,
												timer_clock_t deadline)
{
	deadline_timer->deadline_expired = TIMER_EXPIRED_FALSE;

	timer_clock_clear(&deadline_timer->time_current);
	timer_clock_clear(&deadline_timer->time_initial);
	timer_clock_clear(&deadline_timer->time_goal);
	timer_clock_set_time(&deadline_timer->deadline, deadline);
	return 0;

}


uint8_t deadline_timer_setup_shared_clock(deadline_timer_t *deadline_timer,
												timer_clock_t *time_current,
												timer_clock_t deadline)
{
//	timer_clock_t *temp;

	deadline_timer->deadline_expired = TIMER_EXPIRED_FALSE;



//	deadline_timer->time_current = time_current;
	deadline_timer_assign_current(&deadline_timer->time_current, time_current);

//	timer_clock_clear(&deadline_timer->time_current);
	timer_clock_clear(&deadline_timer->time_initial);
	timer_clock_clear(&deadline_timer->time_goal);
	timer_clock_set_time(&deadline_timer->deadline, deadline);
	return 0;
}


void deadline_timer_assign_current(timer_clock_t *time_current,
											timer_clock_t *time_current_2)
{
	time_current = time_current_2;
}


uint8_t deadline_timer_force_expiration(deadline_timer_t *deadline_timer)
{
	deadline_timer->deadline_expired = TIMER_EXPIRED_TRUE;
	return 0;
}
uint8_t deadline_timer_check(deadline_timer_t *deadline_timer,
								deadline_timer_expired_t *deadline_expired)
{
	uint8_t status = 0;
	//TODO: (High) fix this to include seconds

//	deadline_timer_expired_t deadline_expired_sec = TIMER_EXPIRED_FALSE;
	deadline_timer_expired_t deadline_expired_msec = TIMER_EXPIRED_FALSE;


//	uint32_t time_current = deadline_timer->time_current.msec;
//	uint32_t deadline = deadline_timer->deadline.msec;
//	uint32_t time_initial = deadline_timer->time_initial.msec;

	deadline_timer_compare_check(deadline_timer->time_current.msec,
											deadline_timer->deadline.msec,
											deadline_timer->time_initial.msec,
											DEADLINE_MAX_MSEC,
											&deadline_expired_msec);

	*deadline_expired = deadline_expired_msec;
	deadline_timer->deadline_expired = deadline_expired_msec;
	return status;

}

uint8_t deadline_timer_count(deadline_timer_t *deadline_timer)
{
	deadline_timer_increment(&deadline_timer->time_current);
	return 0;
}

uint8_t deadline_timer_set_current_time(deadline_timer_t *deadline_timer,
													timer_clock_t current_time)
{
	timer_clock_set_time(&deadline_timer->time_current, current_time);
	return 0;
}
uint8_t deadline_timer_set_deadline(deadline_timer_t *deadline_timer,
													timer_clock_t deadline)
{

	timer_clock_set_time(&deadline_timer->deadline, deadline);

	return 0;
}

uint8_t deadline_timer_set_initial_time(deadline_timer_t *deadline_timer)
{
	//memcpy fails after several assignations.
	//copy uint32_t variables directly to avoid a race condition
	deadline_timer->time_initial.counts = deadline_timer->time_current.counts;
	deadline_timer->time_initial.msec = deadline_timer->time_current.msec;


	return 0;
}


uint8_t deadline_timer_get_current_time(deadline_timer_t deadline_timer,
													timer_clock_t *current_time)
{
	*current_time = deadline_timer.time_current;
	return 0;
}
uint8_t deadline_timer_get_deadline(deadline_timer_t deadline_timer,
													timer_clock_t *deadline)
{
	*deadline = deadline_timer.deadline;
	return 0;
}
uint8_t deadline_timer_get_initial_time(deadline_timer_t deadline_timer,
													timer_clock_t *time_initial)
{
	*time_initial = deadline_timer.time_initial;
	return 0;
}


uint8_t timer_clock_clear(timer_clock_t *timer)
{
	timer->counts = 0;
	timer->msec = 0;
	timer->sec = 0;
	return 0;
}

uint8_t timer_clock_set_time(timer_clock_t *timer, timer_clock_t new_time)
{
	timer->msec = new_time.msec;
	timer->sec = new_time.sec;
	return 0;
}

uint8_t timer_clock_get_time(timer_clock_t timer, uint16_t sec, uint16_t msec)
{
	msec = timer.msec;
	sec = timer.sec;
	return 0;
}

uint8_t deadline_timer_increment(timer_clock_t *timer)
{

	timer->msec++;
	timer->counts++;

	if(timer->msec >= DEADLINE_MAX_MSEC )
	{
//		timer->sec++;
		timer->msec = 0;
	}

//	if(timer->sec >= (DEADLINE_MAX_SEC) )
//	{
//		timer->sec = 0;
//	}
//	if(timer->counts >= (DEADLINE_MAX_COUNT) )
//	{
//		timer->counts = 0;
//	}

	return 0;
}




uint8_t deadline_timer_compare_check(uint32_t time_current,
								uint32_t deadline,
								uint32_t time_initial,
								uint32_t max_time,
								deadline_timer_expired_t *deadline_expired)
{
	uint8_t status = 0;
	int64_t remaining = 0;
	int32_t deadline_difference = 0;

	remaining = ((int64_t) time_current) - ((int64_t) time_initial);

	if(remaining < 0)
	{
		remaining = (max_time - time_initial) + time_current;

	}
	deadline_difference = (uint32_t) remaining;

	if(deadline_difference >= deadline)
	{
		*deadline_expired = TIMER_EXPIRED_TRUE;
	}
	else
	{
		*deadline_expired = TIMER_EXPIRED_FALSE;
	}

	return status;
}


//uint8_t deadline_timer_compare_check(uint32_t time_current,
//								uint32_t deadline,
//								uint32_t time_initial,
//								uint32_t max_time,
//								deadline_timer_expired_t *deadline_expired)
//{
//	uint8_t status = 0;
//	int64_t remaining = 0;
//
//	remaining = time_current - time_initial;
//
//	if(remaining < 0)
//	{
//		remaining = (max_time - time_initial) + time_current;
//	}
//
//	if(deadline <= remaining)
//	{
//		*deadline_expired = TIMER_EXPIRED_TRUE;
//	}
//	else
//	{
//		*deadline_expired = TIMER_EXPIRED_FALSE;
//	}
//
//	return status;
//}







//static uint8_t deadline_timer_increment()
//{
//
//}
