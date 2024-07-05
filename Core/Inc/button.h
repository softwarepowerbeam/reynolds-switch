/*
 * button.h
 *
 *  Created on: Jun 10, 2024
 *      Author: Eduardo
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum button_status
{
	BUTTON_OFF = 0,
	BUTTON_ON,
}button_status_t;


typedef enum button_debounce_state_fsm
{
	BUTTON_DEBOUNCE_IDLE = 0,
	BUTTON_DEBOUNCE_WAIT,
	BUTTON_DEBOUNCE_CLEAR_LOCK

}button_debounce_state_fsm_t;

typedef enum button_edge
{
	BUTTON_EDGE_NOT_DETECTED = 0,
	BUTTON_EDGE_NEGATIVE,
	BUTTON_EDGE_POSITIVE
}button_edge_t;

typedef enum button_debounce_lock
{
	BUTTON_DEBOUNCE_LOCK_ON = 0,
	BUTTON_DEBOUNCE_LOCK_OFF,
}button_debounce_lock_t;

typedef enum button_isr_status
{
	BUTTON_ISR_ATTENDED = 0,
	BUTTON_ISR_ATTENDING,
	BUTTON_ISR_UNATTENDED

}button_isr_status_t;

typedef struct button_gpio
{
	GPIO_TypeDef *port;
	uint16_t pin;
}button_gpio_t;

typedef struct button
{
	button_status_t status;
	button_edge_t edge;
	button_isr_status_t edge_attended;
	button_gpio_t hardware_input;

	button_debounce_state_fsm_t debounce_fsm_state;
	button_debounce_lock_t debounce_lock;
	uint32_t debounce_idx;
	uint32_t debounce_count_limit;

}button_t;

//TODO: (low) work on this for push button configuration.
typedef struct push_button
{
	button_status_t status;
	button_status_t last_status;
	button_edge_t edge;
	button_gpio_t hardware_input;


}push_button_t;

//debounce functions
uint8_t button_debounce_fsm(button_t *button);

//basic functions
uint8_t button_setup(button_t *button, button_gpio_t hardware_input);
uint8_t button_get_state(button_t *button);
uint8_t button_positive_edge_detected(button_t *button);
uint8_t button_negative_edge_detected(button_t *button);
uint8_t button_clear_edge_detected(button_t *button);

uint8_t button_check_isr_request(button_t button,
									button_isr_status_t *button_isr_status,
									button_edge_t *edge);
uint8_t button_set_isr_attended(button_t *button);

//Hardware Layer
uint8_t button_get_status(button_t *button, button_status_t *status);


#endif /* INC_BUTTON_H_ */
