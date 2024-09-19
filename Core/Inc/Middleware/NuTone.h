/*
 * NuTone.h
 *
 *  Created on: Aug 30, 2024
 *      Author: Eduardo
 */

#ifndef INC_MIDDLEWARE_NUTONE_H_
#define INC_MIDDLEWARE_NUTONE_H_

#include <stdint.h>
#include "BoardSupport/relay_l114fl_drv8210.h"
#include "Middleware/deadline_timer.h"

#ifdef __cplusplus
extern "C" {
#endif


#define NUTONE_VYV_TIMEOUT	10000


#define NUTONE_WHITE_LIGHT_ON_PULSE_TIME	2500//TODO: (high) find the minimum pulse time
#define NUTONE_WHITE_LIGHT_ON_PULSES		3//!<(always use an even number) find the minimum pulse time

#define NUTONE_VYV_ON_PULSE_TIME	2500//TODO: (high) find the minimum pulse time
#define NUTONE_VYV_ON_PULSES		2//!<(always use an even number) find the minimum pulse time

typedef enum nutone_states
{
	NUTONE_STE_IDLE,
	NUTONE_STE_FAN_SET_ON,
	NUTONE_STE_FAN_TURNING_ON,
	NUTONE_STE_FAN_SET_OFF,
	NUTONE_STE_FAN_TURNING_OFF,
	//White Light
	NUTONE_STE_WHITE_LIGHT_SET_ON,
	NUTONE_STE_WHITE_LIGHT_ON,
	NUTONE_STE_WHITE_LIGHT_OFF,
	NUTONE_STE_WHITE_LIGHT_TURNING_ON,
	//UV light
	NUTONE_STE_VYV_SET_ON,
	NUTONE_STE_VYV_ON,
	NUTONE_STE_VYV_OFF,
	NUTONE_STE_VYV_TURNING_ON,

	//This is a version using the light mode state of the white-vyv lights ctrl
	NUTONE_STE_WHITE_L_SET_ON,
	NUTONE_STE_VYV_L_SET_ON,



	//Both lights modes just require an off pulse
	NUTONE_STE_LIGHTS_OFF,
	NUTONE_STE_LIGHTS_TURNING_OFF,
	NUTONE_STE_LIGHTS_TURNED_OFF,

}nutone_states_e;

typedef enum nutone_cmd_state
{
	NUTONE_CMD_STE_READY,
	NUTONE_CMD_STE_BUSY
}nutone_cmd_state_t;

typedef enum nutone_light_mode
{
	NUTONE_LIGHT_MODE_VYV,
	NUTONE_LIGHT_MODE_WHITE,
	NUTONE_LIGHT_MODE_OFF
}nutone_light_mode_t;

typedef enum nutone_mode
{
	NUTONE_OFF = 0,
	NUTONE_MANUAL_SURFACESHIELD,
	NUTONE_AUTOMATIC_SURFACESHIELD,
	NUTONE_WHITE_LIGHT
}nutone_mode_e;

typedef enum nutone_command
{
//	NUTONE_CMD_TURN_OFF,
	NUTONE_CMD_NONE,
	NUTONE_CMD_FAN_TURN_ON,
	NUTONE_CMD_FAN_TURN_OFF,
	NUTONE_CMD_WHITE_TURN_ON,					//!<Force white light
	NUTONE_CMD_WHITE_TURN_OFF,
	NUTONE_CMD_VYV_TURN_ON,
	NUTONE_CMD_VYV_TURN_OFF,					//!<Vyv light is the antimicrobial Light technology

	//As in the instruction manual:
	NUTONE_CMD_MANUAL_SURFACESHIELD_SET,		//!<Execute the activation of the manual SurfaceShield mode as described in the documentation
	NUTONE_CMD_MANUAL_SURFACESHIELD_CLEAR,
	NUTONE_CMD_AUTOMATIC_SURFACESHIELD_SET,
	NUTONE_CMD_AUTOMATIC_SURFACESHIELD_CLEAR,
	NUTONE_CMD_WHITE_LIGHT_SET,
	NUTONE_CMD_WHITE_LIGHT_CLEAR,

}nutone_command_e;



typedef struct nutone_relay_handler
{
	relay_t *relay;
	deadline_timer_t *deadline_timer;
}nutone_relay_handler_t;

typedef struct nutone
{
//	relay_t *lights;
//	relay_t *fan;
	nutone_relay_handler_t *lights;
	nutone_relay_handler_t *fan;
	deadline_timer_t *ctrl_timer;
	deadline_timer_t *vyv_timeoff;
	nutone_states_e fsm_state;
	nutone_mode_e op_mode;
	nutone_command_e command;
	nutone_cmd_state_t cmd_state;
	uint8_t light_on_pulses_idx;
	uint8_t light_on_pulses;
	nutone_light_mode_t current_light_mode;
	nutone_light_mode_t next_light_mode;
}nutone_t;



void nutone_setup(nutone_t *exhaust_fan, nutone_relay_handler_t *lights,
											nutone_relay_handler_t *fan,
											deadline_timer_t *ctrl_timer,
											deadline_timer_t *vyv_timeoff);

uint8_t nutone_fsm(nutone_t *exhaust_fan);

uint8_t nutone_set_mode(nutone_t *exhaust_fan, nutone_mode_e op_mode);

uint8_t nutone_set_command(nutone_t *exhaust_fan, nutone_command_e command);

#ifdef __cplusplus
}
#endif

#endif /* INC_MIDDLEWARE_NUTONE_H_ */
