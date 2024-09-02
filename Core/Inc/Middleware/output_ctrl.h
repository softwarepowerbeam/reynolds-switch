/*
 * output_ctrl.hpp
 *
 *  Created on: Aug 29, 2024
 *      Author: Eduardo
 */




#ifndef INC_MIDDLEWARE_OUTPUT_CTRL_H_
#define INC_MIDDLEWARE_OUTPUT_CTRL_H_

#include <stdint.h>
#include "BoardSupport/relay_l114fl_drv8210.h"
#include "Middleware/deadline_timer.h"

#ifdef __cplusplus
extern "C" {
#endif


void output_fsm_ctrl(relay_t *actuator, deadline_timer_t *deadline_timer);



#ifdef __cplusplus
}
#endif


#endif /* INC_MIDDLEWARE_OUTPUT_CTRL_H_ */
