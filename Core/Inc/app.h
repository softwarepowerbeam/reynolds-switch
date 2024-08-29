/*
 * app.h
 *
 *  Created on: Aug 29, 2024
 *      Author: Eduardo
 */

#ifndef INC_APP_H_
#define INC_APP_H_


typedef enum boolean_values
{
	FALSE = 0,
	TRUE,
}boolean_values_t;

typedef struct timer
{
	uint32_t time_msec;
	uint32_t time_sec;
}timer_t;

typedef enum action
{
	SET_VALUE = 0,
	GET_VALUE,
}action_e;







#endif /* INC_APP_H_ */
