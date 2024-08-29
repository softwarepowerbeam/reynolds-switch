/*
 * switch_selector.c
 *
 *  Created on: Jun 17, 2024
 *      Author: Eduardo
 */


#include "BoardSupport/switch_selector.h"

uint8_t switch_selector_setup(switch_selector_t *switch_selector,
								switch_selector_gpio_t *switch_selector_gpio,
								uint8_t switch_size)
{

	uint8_t status = 0;
	uint8_t idx = 0;



	if(switch_size > 32)
	{
		return 2;
	}


	switch_selector->value = 0;
	switch_selector->switch_size = switch_size;
	switch_selector->switch_selector_gpio = (switch_selector_gpio_t*)malloc(switch_size * sizeof(switch_selector_gpio_t) );
	uint32_t value;
	switch_selector_status_t pin_status;

	for(idx = 0; idx < switch_size; idx++)
	{
		if((switch_selector_gpio + idx) != NULL)
		{
//			switch_selector->switch_selector_gpio[idx] = switch_selector_gpio[idx];
			*(switch_selector->switch_selector_gpio + idx) = *(switch_selector_gpio + idx);

			status = 0;
		}
		else
		{
			status = 1;
			break;
		}

		switch_selector_read_bit(switch_selector, idx, &pin_status, &value);

	}

	return status;

}

uint8_t switch_selector_deinit(switch_selector_t *switch_selector)
{
	free(switch_selector->switch_selector_gpio);
	return 0;
}


uint8_t switch_selector_read(switch_selector_t *switch_selector,
													uint32_t *value)
{
	switch_selector_status_t pin_status;
	uint8_t idx;

	for(idx = 0; idx < switch_selector->switch_size; idx++)
	{
		switch_selector_read_bit(switch_selector, idx, &pin_status, value);
	}

	return 0;
}

uint8_t switch_selector_read_bit(switch_selector_t *switch_selector,
										uint8_t pin,
										switch_selector_status_t *pin_status,
										uint32_t *value)
{
	GPIO_PinState pin_value;
	uint32_t switch_mask = 0x00000001;

	switch_mask <<= pin;

	pin_value = HAL_GPIO_ReadPin((switch_selector->switch_selector_gpio + pin)->port,
							(switch_selector->switch_selector_gpio + pin)->pin);

	if(pin_value == GPIO_PIN_SET)
	{
		(switch_selector->switch_selector_gpio + pin)->status = SWITCH_SELECTOR_ON;

		switch_selector->value |= switch_mask;
	}
	else
	{
		(switch_selector->switch_selector_gpio + pin)->status = SWITCH_SELECTOR_OFF;

		switch_selector->value &= ~switch_mask;
	}

	*pin_status = (switch_selector->switch_selector_gpio + pin)->status;

	*value = switch_selector->value;

	return 0;
}
