/*
 * led_indicator.h
 *
 *  Created on: Jun 14, 2024
 *      Author: Eduardo
 */

#ifndef INC_LED_INDICATOR_H_
#define INC_LED_INDICATOR_H_
#include "gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum led_signal_type
{
	LED_SIGNAL_OFF = 0,
	LED_SIGNAL_SOLID,
	LED_SIGNAL_BLINK,
	LED_SIGNAL_BLINK_2
	//TODO: (add others)
}led_signal_type_t;


typedef enum led_signal_ctrl
{
	LED_SIGNAL_CTRL_START = 0,
	LED_SIGNAL_CTRL_STOP
}led_signal_ctrl_t;

typedef enum led_signal_fsm_state
{
	LED_SIGNAL_STATE_IDDLE = 0,
	LED_SIGNAL_STATE_ON,
	LED_SIGNAL_STATE_SOLID_ON,

	//TODO: (add others)

	LED_SIGNAL_STATE_TOGLE,
	LED_SIGNAL_STATE_END
}led_signal_fsm_state_t;


typedef struct led_signal_gpio
{
	GPIO_TypeDef *port;
	uint16_t pin;
}led_signal_gpio_t;




typedef struct led_signal
{
	led_signal_gpio_t gpio;
	led_signal_ctrl_t control;
	led_signal_type_t type;
	led_signal_fsm_state_t state;
}led_signal_t;



uint8_t led_signal_fsm(led_signal_t *led_signal);
uint8_t led_signal_setup(led_signal_t *led_signal, led_signal_gpio_t gpio);
uint8_t led_signal_type_selector(led_signal_t *led_signal,
												led_signal_type_t type);
//Operation modes
uint8_t led_signal_solid_fsm(led_signal_t *led_signal);
uint8_t led_signal_togle_fsm(led_signal_t *led_signal);

//Control signal LED
uint8_t led_signal_start(led_signal_t *led_signal);
uint8_t led_signal_stop(led_signal_t *led_signal);

uint8_t led_signal_turn_on(led_signal_t led_signal);
uint8_t led_signal_turn_off(led_signal_t led_signal);
uint8_t led_signal_toggle(led_signal_t led_signal);

#ifdef __cplusplus
}
#endif

#endif /* INC_LED_INDICATOR_H_ */
