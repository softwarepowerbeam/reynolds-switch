/*
 * button.c
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */


#include "BoardSupport/button.h"


uint8_t button_setup(button_t *button, button_gpio_t hardware_input)
{

	button->hardware_input = hardware_input;
	button->edge = BUTTON_EDGE_NOT_DETECTED;
	button->edge_attended = BUTTON_ISR_ATTENDED;
	button->debounce_fsm_state = BUTTON_DEBOUNCE_IDLE;
	button->debounce_lock = BUTTON_DEBOUNCE_LOCK_OFF;
	button->debounce_idx = 0;
	button->debounce_count_limit = 1;
	button->push_status = BUTTON_PUSH_OFF;//For push button only

	return 0;

}


uint8_t button_debounce_fsm(button_t *button)
{
	button_debounce_state_fsm_t debounce_fsm_state = button->debounce_fsm_state;
	button_edge_t edge = button->edge;
	uint32_t debounce_idx = button->debounce_idx;
	uint32_t debounce_count_limit = button->debounce_count_limit;
	button_status_t button_status;

	switch(debounce_fsm_state)
	{
		case BUTTON_DEBOUNCE_IDLE:

			if(button->debounce_lock == BUTTON_DEBOUNCE_LOCK_ON)
			{
				if(debounce_count_limit == 0)
				{
					debounce_fsm_state = BUTTON_DEBOUNCE_CLEAR_LOCK;
				}
				else
				{
					debounce_fsm_state = BUTTON_DEBOUNCE_WAIT;
				}
			}
			break;
		case BUTTON_DEBOUNCE_WAIT:

			if(debounce_idx >= debounce_count_limit)
			{
				debounce_fsm_state = BUTTON_DEBOUNCE_CLEAR_LOCK;
			}
			debounce_idx++;

			break;
		case BUTTON_DEBOUNCE_CLEAR_LOCK:

			button->edge_attended = BUTTON_ISR_UNATTENDED;
			button->debounce_idx = 0;
			button->debounce_lock = BUTTON_DEBOUNCE_LOCK_OFF;
			button_get_status(button, &button_status);

			debounce_fsm_state = BUTTON_DEBOUNCE_IDLE;

			break;
		default:
			debounce_fsm_state = BUTTON_DEBOUNCE_IDLE;
			break;
	}

	button->debounce_fsm_state = debounce_fsm_state;
	button->edge = edge;
	button->debounce_idx = debounce_idx;

	return 0;
}






uint8_t button_get_state(button_t *button)
{
	GPIO_PinState pin_value = GPIO_PIN_RESET;


	pin_value = HAL_GPIO_ReadPin(button->hardware_input.port,
										button->hardware_input.pin);

	if(pin_value == GPIO_PIN_RESET)
	{
		button->status = BUTTON_OFF;
	}
	else
	{
		button->status = BUTTON_ON;
	}

	return 0;

}


uint8_t button_positive_edge_detected(button_t *button)
{

	if(button->debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
	{
		button->edge = BUTTON_EDGE_POSITIVE;
		button->debounce_lock = BUTTON_DEBOUNCE_LOCK_ON;
	}
	return 0;
}

uint8_t button_negative_edge_detected(button_t *button)
{
	if(button->debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
	{
		button->edge = BUTTON_EDGE_NEGATIVE;
		button->debounce_lock = BUTTON_DEBOUNCE_LOCK_ON;
	}
	return 0;
}

uint8_t button_clear_debounce_lock(button_t *button)
{
	button->debounce_lock = BUTTON_DEBOUNCE_LOCK_OFF;
	return 0;
}
uint8_t button_clear_edge_detected(button_t *button)
{
	button->edge = BUTTON_EDGE_NOT_DETECTED;
	return 0;
}

uint8_t button_get_status(button_t *button, button_status_t *status)
{
	GPIO_PinState pin_value;
	pin_value = HAL_GPIO_ReadPin(button->hardware_input.port,
												button->hardware_input.pin);
	if(pin_value == GPIO_PIN_SET)
	{
		button->status = BUTTON_ON;
	}
	else
	{
		button->status = BUTTON_OFF;
	}

	*status = button->status;

	return 0;
}

uint8_t button_set_isr_attended(button_t *button)
{
	button->edge = BUTTON_EDGE_NOT_DETECTED;
	button->edge_attended = BUTTON_ISR_ATTENDED;
	return 0;
}


uint8_t button_check_isr_request(button_t button,
									button_isr_status_t *button_isr_status,
									button_edge_t *edge)
{
	*button_isr_status = button.edge_attended;
	*edge = button.edge;
	return 0;
}

