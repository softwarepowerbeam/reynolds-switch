/*
 * switch_selector.h
 *
 *  Created on: Jun 17, 2024
 *      Author: Eduardo
 */

#ifndef INC_SWITCH_SELECTOR_H_
#define INC_SWITCH_SELECTOR_H_


#include "gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum switch_selector_status
{
	SWITCH_SELECTOR_OFF = 0,
	SWITCH_SELECTOR_ON,
}switch_selector_status_t;

typedef struct switch_selector_gpio
{
	GPIO_TypeDef *port;
	uint16_t pin;
	switch_selector_status_t status;
}switch_selector_gpio_t;



typedef struct switch_selector
{
	switch_selector_gpio_t *switch_selector_gpio;
	uint8_t switch_size;
	uint32_t value;//!<whole selector (dip switch) value
}switch_selector_t;

uint8_t switch_selector_setup(switch_selector_t *switch_selector,
								switch_selector_gpio_t *switch_selector_gpio,
								uint8_t switch_size);

uint8_t switch_selector_deinit(switch_selector_t *switch_selector);

uint8_t switch_selector_read_bit(switch_selector_t *switch_selector,
										uint8_t pin,
										switch_selector_status_t *pin_status,
										uint32_t *value);

uint8_t switch_selector_read(switch_selector_t *switch_selector,
													uint32_t *value);

#ifdef __cplusplus
}
#endif

#endif /* INC_SWITCH_SELECTOR_H_ */
