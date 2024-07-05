/*
 * motion_lights_driver.h
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */

#ifndef INC_MOTION_LIGHTS_DRIVER_H_
#define INC_MOTION_LIGHTS_DRIVER_H_


#include "pyd15X8.h"
#include "deadline_timer.h"
#include "button.h"
#include "lights.h"


typedef enum motion_switch_op_mode
{
	MOTION_SWITCH_MODE_0 = 0,	//!<Two motion lights; one UV light
	MOTION_SWITCH_MODE_1,		//!<One motion light; one UV light; one direct light
	MOTION_SWITCH_MODE_2,		//!<Not assigned: do nothing
	MOTION_SWITCH_MODE_3		//!<Not assigned: do nothing
}motion_switch_op_mode_t;

typedef enum motion_light_state
{
	MOTION_LIGHT_IDLE = 0,
	MOTION_LIGHT_CHECK_BUTTON,
	MOTION_LIGHT_TURN_ON_LIGHT,
	MOTION_LIGHT_INIT_TIMER,
	MOTION_LIGHT_WAIT_EXPIRATION,
	MOTION_LIGHT_TURN_OFF_LIGHT
}motion_light_state_t;


typedef enum motion_light_uv_state
{
	MOTION_LIGHT_UV_IDLE = 0,
	MOTION_LIGHT_UV_INIT_SAFE_TIMER,
	MOTION_LIGHT_UV_WAIT_SAFE_TIMER,
	MOTION_LIGHT_UV_TURN_ON_LIGHT,
	MOTION_LIGHT_UV_INIT_TIMER,
	MOTION_LIGHT_UV_WAIT_EXPIRATION,
	MOTION_LIGHT_UV_TURN_OFF_LIGHT,
	MOTION_LIGHT_UV_ABORT
}motion_light_uv_state_t;

typedef enum motion_light_no_motion_state
{
	NO_MOTION_LIGHT_IDLE = 0,
	NO_MOTION_LIGHT_CHECK_BUTTON,
	NO_MOTION_LIGHT_TURN_ON_LIGHT,
	NO_MOTION_LIGHT_TURN_OFF_LIGHT
}motion_light_no_motion_state_t;


typedef enum motion_light_uv_abort
{
	MOTION_ABORT_TRUE,
	MOTION_ABORT_FALSE
}motion_light_uv_abort_t;


typedef enum motion_sensed_t
{
	MOTION_ISR_ATTENDED = 0,
	MOTION_ISR_ATTENDING,
	MOTION_ISR_UNATTENDED//TODO: (low) create unattended timeout?
}motion_sensed_t;

typedef struct motion_light
{
	light_t light;
	button_t button;
	pyd1598_sensor_t sensor;
	deadline_timer_t deadline_timer;
	motion_light_state_t light_fsm_state;
	motion_sensed_t motion_sensed;
}motion_light_t;



//typedef struct motion_switch
//{
////	light_t light;
////	button_t button;
////	pyd1598_sensor_t sensor;
////	deadline_timer_t deadline_timer;
////	pyd1598_pulse_counter_t pulse_counter;
//	motion_sensed_t semaphore_light;
//	uint16_t semaphore_uv;
//}motion_switch_t;





#endif /* INC_MOTION_LIGHTS_DRIVER_H_ */
