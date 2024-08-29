/*
 * timer_ms.h
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */

#ifndef INC_DEADLINE_TIMER_H_
#define INC_DEADLINE_TIMER_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

//Precondition: a task running every 1ms

//maximum number of micro seconds is 1 sec
//const uint16_t DEADLINE_MAX_MSEC = 1000;
//const uint64_t DEADLINE_MAX_COUNT = 1000000000000000000;
//const uint16_t DEADLINE_MAX_SEC = 60000;

#define DEADLINE_MAX_MSEC 	(uint32_t) 1000000000
//#define DEADLINE_MAX_MSEC 	(uint32_t) 20000
#define DEADLINE_MAX_COUNT 	(uint64_t) 1000000000000000000
#define DEADLINE_MAX_SEC 	(uint32_t) 60000


typedef enum deadline_timer_expired
{
	TIMER_EXPIRED_FALSE = 0,
	TIMER_EXPIRED_TRUE
}deadline_timer_expired_t;

typedef struct timer_clock
{
	uint64_t counts;//TODO: (high) should I implement this?
	uint32_t msec;
	uint32_t sec;//TODO: (high) should I implement this?
	uint32_t min;//TODO: (high) should I implement this?

}timer_clock_t;



typedef struct deadline_timer
{
	timer_clock_t time_current;
	timer_clock_t deadline;
	timer_clock_t time_initial;
	timer_clock_t time_goal;
	deadline_timer_expired_t deadline_expired;
}deadline_timer_t;

uint8_t deadline_timer_setup(deadline_timer_t *deadline_timer,
												timer_clock_t deadline);
uint8_t deadline_timer_setup_shared_clock(deadline_timer_t *deadline_timer,
												timer_clock_t *time_current,
												timer_clock_t deadline);
uint8_t deadline_timer_count(deadline_timer_t *deadline_timer);

uint8_t deadline_timer_check(deadline_timer_t *deadline_timer,
									deadline_timer_expired_t *deadline_expired);

uint8_t deadline_timer_force_expiration(deadline_timer_t *deadline_timer);

uint8_t deadline_timer_set_current_time(deadline_timer_t *deadline_timer,
													timer_clock_t current_time);
uint8_t deadline_timer_set_deadline(deadline_timer_t *deadline_timer,
													timer_clock_t deadline);
uint8_t deadline_timer_set_initial_time(deadline_timer_t *deadline_timer);

uint8_t deadline_timer_get_current_time(deadline_timer_t deadline_timer,
												timer_clock_t *current_time);
uint8_t deadline_timer_get_deadline(deadline_timer_t deadline_timer,
													timer_clock_t *deadline);
uint8_t deadline_timer_get_initial_time(deadline_timer_t deadline_timer,
												timer_clock_t *time_initial);


uint8_t timer_clock_clear(timer_clock_t *timer);
uint8_t timer_clock_set_time(timer_clock_t *timer, timer_clock_t new_time);
uint8_t timer_clock_set_time(timer_clock_t *timer, timer_clock_t new_time);
uint8_t deadline_timer_increment(timer_clock_t *timer);


uint8_t deadline_timer_compare_check(uint32_t time_current,
								uint32_t deadline,
								uint32_t time_initial,
								uint32_t max_time,
								deadline_timer_expired_t *deadline_expired);
void deadline_timer_assign_current(timer_clock_t *time_current,
											timer_clock_t *time_current_2);


#ifdef __cplusplus
}
#endif

#endif /* INC_DEADLINE_TIMER_H_ */
