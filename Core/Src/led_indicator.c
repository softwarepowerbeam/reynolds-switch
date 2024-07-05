/*
 * led_indicator.c
 *
 *  Created on: Jun 14, 2024
 *      Author: Eduardo
 */

#include "led_indicator.h"

uint8_t led_signal_setup(led_signal_t *led_signal, led_signal_gpio_t gpio)
{
	led_signal->gpio = gpio;
	led_signal->type = LED_SIGNAL_OFF;

	return 0;
}



uint8_t led_signal_fsm(led_signal_t *led_signal)
{
	led_signal_type_t type = led_signal->type;

	switch(type)
	{
		case LED_SIGNAL_OFF:
			break;
		case LED_SIGNAL_SOLID:
			led_signal_solid_fsm(led_signal);
			break;
		case LED_SIGNAL_BLINK:
			led_signal_togle_fsm(led_signal);
			break;
		case LED_SIGNAL_BLINK_2:
			break;
		default:
			break;
	}

	return 0;
}




uint8_t led_signal_type_selector(led_signal_t *led_signal,
												led_signal_type_t type)
{
	led_signal->type = type;
	return 0;
}

uint8_t led_signal_solid_fsm(led_signal_t *led_signal)
{
	led_signal_fsm_state_t state = led_signal->state;
	led_signal_ctrl_t control = led_signal->control;


	switch(state)
	{
		case LED_SIGNAL_STATE_IDDLE:
			if(control == LED_SIGNAL_CTRL_START)
			{
				state = LED_SIGNAL_STATE_ON;
			}
			break;
		case LED_SIGNAL_STATE_ON:
			led_signal_turn_on(*led_signal);
			state = LED_SIGNAL_STATE_SOLID_ON;
			break;
		case LED_SIGNAL_STATE_SOLID_ON:

			if(control == LED_SIGNAL_CTRL_STOP)
			{
				state = LED_SIGNAL_STATE_END;
			}
			break;
		case LED_SIGNAL_STATE_END:
			led_signal_turn_off(*led_signal);
			state = LED_SIGNAL_STATE_IDDLE;
			break;
		default:
			state = LED_SIGNAL_STATE_IDDLE;
			break;

	}

	led_signal->state = state;

	return 0;
}

uint8_t led_signal_togle_fsm(led_signal_t *led_signal)
{
	led_signal_fsm_state_t state = led_signal->state;
	led_signal_ctrl_t control = led_signal->control;


	switch(state)
	{
		case LED_SIGNAL_STATE_IDDLE:
			if(control == LED_SIGNAL_CTRL_START)
			{
				state = LED_SIGNAL_STATE_ON;
			}
			break;
		case LED_SIGNAL_STATE_ON:
			led_signal_turn_on(*led_signal);
			state = LED_SIGNAL_STATE_TOGLE;
			break;
		case LED_SIGNAL_STATE_TOGLE:

			led_signal_toggle(*led_signal);
			if(control == LED_SIGNAL_CTRL_STOP)
			{
				state = LED_SIGNAL_STATE_END;
			}
			break;
		case LED_SIGNAL_STATE_END:
			led_signal_turn_off(*led_signal);
			state = LED_SIGNAL_STATE_IDDLE;
			break;
		default:
			state = LED_SIGNAL_STATE_IDDLE;
			break;

	}

	led_signal->state = state;

	return 0;
}


uint8_t led_signal_start(led_signal_t *led_signal)
{
	led_signal->control = LED_SIGNAL_CTRL_START;

	return 0;
}
uint8_t led_signal_stop(led_signal_t *led_signal)
{
	led_signal->control = LED_SIGNAL_CTRL_STOP;

	return 0;
}

uint8_t led_signal_turn_on(led_signal_t led_signal)
{
	HAL_GPIO_WritePin(led_signal.gpio.port, led_signal.gpio.pin, GPIO_PIN_SET);
	return 0;
}

uint8_t led_signal_turn_off(led_signal_t led_signal)
{
	HAL_GPIO_WritePin(led_signal.gpio.port, led_signal.gpio.pin,
													GPIO_PIN_RESET);
	return 0;
}

uint8_t led_signal_toggle(led_signal_t led_signal)
{
	HAL_GPIO_TogglePin(led_signal.gpio.port, led_signal.gpio.pin);
	return 0;
}
