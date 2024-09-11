/*
 * nutone_app.h
 *
 *  Created on: Sep 4, 2024
 *      Author: Eduardo
 */



#include "BoardSupport/pyd15X8.h"
#include "BoardSupport/button.h"
#include "BoardSupport/relay_l114fl_drv8210.h"
#include "BoardSupport/led_indicator.h"

#include "Middleware/Nutone.h"
#include "Application/motion_lights_driver.h"
#include "Middleware/output_ctrl.h"

#ifndef INC_APPLICATION_NUTONE_APP_H_
#define INC_APPLICATION_NUTONE_APP_H_



//typedef enum nutone_app_state_white
//{
//
//}nutone_app_state_white_t;
//
//typedef enum nutone_app_state_vyv
//{
//
//}nutone_app_state_vyv_t;
//
//typedef enum nutone_app_state_fan
//{
//
//}nutone_app_state_fan_t;


typedef struct button_handler
{
	button_t *button;
	deadline_timer_t *btn_timer;
}button_handler_t;

typedef struct motion_handler
{
	pyd1598_sensor_t *motion_sensor;
	deadline_timer_t *motion_timer;
}motion_handler_t;

typedef struct relay_handler
{
	relay_t *relay;
	deadline_timer_t *relay_timer;
}relay_handler_t;

typedef struct led_signal_handler
{
	led_signal_t *led_signal;
	deadline_timer_t *led_signal_timer;
}led_signal_handler_t;

typedef struct nutone_app_signals
{
	motion_sensed_t motion_light;
	motion_sensed_t motion_uv;
	motion_light_uv_abort_t uv_abort;
	motion_uv_wait_signal_t wait;
}nutone_app_signals_t;

typedef struct nutone_app
{
	//Devices
	nutone_t *nutone_dev;
	motion_handler_t *motion_hand;
	button_handler_t *button_fan;
	button_handler_t *button_white;
	button_handler_t *button_vyv;
	relay_handler_t *lights;
	relay_handler_t *fan;
	led_signal_handler_t *led_signal_hand;

	deadline_timer_t *timer_fsm_white;
	deadline_timer_t *timer_fsm_vyv;
	deadline_timer_t *timer_fsm_vyv_timeout;
	deadline_timer_t *timer_fsm_vyv_safe;
	deadline_timer_t *timer_fsm_fan;
	//Application state machine
	nutone_app_signals_t signals;
	motion_light_state_t fsm_state_white;
	motion_light_uv_state_t fsm_state_uyu;

}nutone_app_t;





uint8_t nutone_app_process(nutone_app_t *nutone_app_hand);

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
							deadline_timer_t *timer_fsm_vyv_safe);

void nutone_app_check_events(nutone_app_t *nutone_app_hand);

uint8_t nutone_app_fsm(nutone_app_t *nutone_app_hand);

#endif /* INC_APPLICATION_NUTONE_APP_H_ */
