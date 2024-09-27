/*
 * PYD15X8.h
 *
 *  Created on: Jun 5, 2024
 *      Author: Eduardo
 */

#ifndef INC_PYD15X8_H_
#define INC_PYD15X8_H_



#include "gpio.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************Data types declarations***************************/
typedef enum pyd1598_blind_time
{
	PYD1598_BT_0_5_SEC = 0,
	PYD1598_BT_1_SEC,
	PYD1598_BT_1_5_SEC,
	PYD1598_BT_2_SEC,
	PYD1598_BT_2_5_SEC,
	PYD1598_BT_3_SEC,
	PYD1598_BT_3_5_SEC,
	PYD1598_BT_4_SEC,
	PYD1598_BT_4_5_SEC,
	PYD1598_BT_5_SEC,
	PYD1598_BT_5_5_SEC,
	PYD1598_BT_6_SEC,
	PYD1598_BT_6_5_SEC,
	PYD1598_BT_7_SEC,
	PYD1598_BT_7_5_SEC,
	PYD1598_BT_8_SEC
}pyd1598_blind_time_t;

typedef enum pyd1598_pulse_counter
{
	PYD1598_PC_1_PULSE = 0,
	PYD1598_PC_2_PULSES,
	PYD1598_PC_3_PULSES,
	PYD1598_PC_4_PULSES
}pyd1598_pulse_counter_t;

typedef enum pyd1598_window_time
{
	PYD1598_WT_2_SEC = 0,
	PYD1598_WT_4_SEC,
	PYD1598_WT_6_SEC,
	PYD1598_WT_8_SEC
}pyd1598_window_time_t;

typedef enum pyd1598_op_mode
{
	PYD1598_FORCE_READOUT = 0,
	PYD1598_INTERRUPT_READOUT = 1,
	PYD1598_WAKE_UP = 2,
	PYD1598_RESERVED = 3,
}pyd1598_op_mode_t;

typedef enum pyd1598_signal_source
{
	PYD1598_SOURCE_PIR_BFP = 0,
	PYD1598_SOURCE_PIR_LPF = 1,
	PYD1598_SOURCE_RESERVED = 2,
	PYD1598_SOURCE_TEMP = 3,
}pyd1598_signal_source_t;

typedef enum pyd1598_hpf_cutoff
{
	PYD1598_HPF_0_4HZ = 0,
	PYD1598_HPF_0_2HZ = 1
}pyd1598_hpf_cutoff_t;

typedef enum pyd1598_count_mode
{
	PYD1598_COUNT_MODE_NO_BPF_SIGN_CHANGE = 0,
	PYD1598_COUNT_MODE_BPF_SIGN_CHANGE = 1
}pyd1598_count_mode_t;

typedef enum pyd1598_out_of_range
{
	PYD1598_OUT_OF_RANGE_NORMAL = 0,
	PYD1598_OUT_OF_RANGE_RESET
}pyd1598_out_of_range_t;

typedef enum pyd1598_interrupt_readout_state
{
	PYD_STATE_INTERRUPT_DRIVE_ZERO,
	PYD_STATE_INTERRUPT_READOUT

}pyd1598_interrupt_readout_state_t;

typedef enum pyd1598_wakeup_state
{
	PYD_STATE_WAKEUP_DRIVE_ZERO,
	PYD_STATE_WAKEUP_READOUT

}pyd1598_wakeup_state_t;



typedef enum pyd1598_wakeup_conf
{
	READOUT_WAKEUP_READ = 0,
	READOUT_WAKEUP_NO_READ
}pyd1598_wakeup_conf_t;

typedef enum pyd1598_forced_readout_state
{
	PYD_STATE_FORCED_DRIVE_ONE = 0,
	PYD_STATE_FORCED_READOUT

}pyd1598_forced_readout_state_t;

typedef enum pyd1598_readout_status
{
	READOUT_READY = 0,
	READOUT_BUSY
}pyd1598_readout_status_t;


typedef enum pyd1598_serin_state
{
	PYD1598_SERIN_IDLE = 0,
	PYD1598_SERIN_SETUP_FOR_MESSAGE,
	PYD1598_SERIN_SEND_MSG,
	PYD1598_SERIN_SEND_DOWN,
	PYD1598_SERIN_WAIT_LOAD_TIME,//Data Load Time tSLT  650μs
	PYD1598_SERIN_END_COM

}pyd1598_serin_state_t;

typedef enum pyd1598_readout_state
{
	PYD1598_READOUT_IDLE = 0,
	PYD1598_READOUT_GET_MSG,
	PYD1598_READOUT_UPDATE,
	PYD1598_READOUT_SIGNAL_DOWN,
	PYD1598_READOUT_END_COM
}pyd1598_readout_state_t;


typedef enum pyd1598_wakeup_isr_status
{
	PYD1598_WAKEUP_ISR_ATTENDED = 0,
	PYD1598_WAKEUP_ISR_ATTENDING,
	PYD1598_WAKEUP_ISR_UNATTENDED

}pyd1598_wakeup_isr_status_t;

typedef enum pyd1598_motion_isr_status
{
	PYD1598_MOTION_ISR_ATTENDED = 0,
	PYD1598_MOTION_ISR_ATTENDING,
	PYD1598_MOTION_ISR_UNATTENDED//TODO: (low) create unattended timeout?

}pyd1598_motion_isr_status_t;


typedef struct pyd1598_config
{
	uint8_t threshold;
	pyd1598_blind_time_t blind_time;
	pyd1598_pulse_counter_t pulse_counter;
	pyd1598_window_time_t window_time;
	pyd1598_op_mode_t op_mode;
	pyd1598_signal_source_t signal_source;
	pyd1598_hpf_cutoff_t hpf_cutoff;
	pyd1598_count_mode_t count_mode;
}pyd1598_config_t;

typedef struct pyd1598_source_value
{
	uint16_t pir_bpf;
	uint16_t pir_lpf;
	uint16_t temperature;

}pyd1598_source_value_t;



typedef struct pyd1598_hardware_interface
{
	GPIO_TypeDef *port;
	uint16_t pin;
	IRQn_Type irq_type;
}pyd1598_hardware_interface_t;



typedef struct pyd1598_direct_link
{
	//Information variables
	pyd1598_config_t config;
	pyd1598_source_value_t source_value;
	pyd1598_out_of_range_t status;
	uint64_t datagram_bufffer;
	uint32_t datagram_config;

	//FSM variables
	pyd1598_forced_readout_state_t forced_state;
	pyd1598_wakeup_conf_t wakeup_mode;
	pyd1598_wakeup_state_t wakeup_state;
//	pyd1598_interrupt_readout_state_t interrupt_state;
	pyd1598_readout_status_t readout_status;
	pyd1598_readout_state_t readout_state;
	bool start_fsm;
	uint8_t time_update_idx;

	//Wake-up interrupt variables
	pyd1598_wakeup_isr_status_t wakeup_isr_status;

	//hardware interface
	pyd1598_hardware_interface_t hardware_inteface;


}pyd1598_direct_link_t;


typedef struct pyd1598_serin
{
	//Information variables
	pyd1598_config_t config;
	uint32_t conf_datagram;

	//FSM variables
	uint32_t mask;
	uint32_t data_idx;
	pyd1598_serin_state_t state;
	bool start_fsm;

	//hardware interface
	pyd1598_hardware_interface_t hardware_inteface;

}pyd1598_serin_t;

typedef struct pyd1598_sensor
{
	pyd1598_serin_t serin;
	pyd1598_direct_link_t direct_link;

	//configuration confirmed from a message read from the sensor
	pyd1598_config_t current_config;
	pyd1598_motion_isr_status_t motion_sensed;

}pyd1598_sensor_t;


/********************************Macros declarations***************************/


//SERIN masks
#define PYD1598_SERIN_MSG_LONG			25
#define PYD1598_SERIN_BIT_24_MASK		0x01000000
#define PYD1598_SERIN_COUNT_TSLT		PYD1598_SERIN_MSG_LONG + 10//interrupts are 100ms so 7 pulses takes 700us

#define PYD1598_SERIN_MASK				0x01FFFFFF

#define PYD1598_SHIFT_THRESHOLD			17
#define PYD1598_MASK_THRESHOLD			0x0001FFFF//!< command should change the bits 17:24

#define PYD1598_SHIFT_BLIND_TIME		13
#define PYD1598_MASK_BLIND_TIME			(PYD1598_SERIN_MASK & ~(0x0000000F << PYD1598_SHIFT_BLIND_TIME))

#define PYD1598_SHIFT_PULSE_COUNTER		11
#define PYD1598_MASK_PULSE_COUNTER		(PYD1598_SERIN_MASK & ~(0x00000003 << PYD1598_SHIFT_PULSE_COUNTER))

#define PYD1598_SHIFT_WINDOW_TIME		9
#define PYD1598_MASK_WINDOW_TIME		(PYD1598_SERIN_MASK & ~(0x00000003 << PYD1598_SHIFT_WINDOW_TIME))

#define PYD1598_SHIFT_OPERATION_MODES	7
#define PYD1598_MASK_OPERATION_MODES	(uint32_t) (PYD1598_SERIN_MASK & (~(0x00000003 << PYD1598_SHIFT_OPERATION_MODES)))

#define PYD1598_SHIFT_SIGNAL_SOURCE 	5
#define PYD1598_MASK_SIGNAL_SOURCE		(PYD1598_SERIN_MASK & ~(0x00000003 << PYD1598_SHIFT_SIGNAL_SOURCE))

#define PYD1598_SHIFT_RESERVED			3
#define PYD1598_MASK_RESERVED			(PYD1598_SERIN_MASK & ~(0x00000003 << PYD1598_SHIFT_RESERVED))

#define PYD1598_SHIFT_HPF_CUTOFF		2
#define PYD1598_MASK_HPF_CUTOFF			(PYD1598_SERIN_MASK & ~(0x00000001 << PYD1598_SHIFT_HPF_CUTOFF))

#define PYD1598_SHIFT_RESERVED2			1
#define PYD1598_MASK_RESERVED2			(PYD1598_SERIN_MASK & ~(0x00000001 << PYD1598_SHIFT_RESERVED2))

#define PYD1598_SHIFT_COUNT_MODE 		0
#define PYD1598_MASK_COUNT_MODE			(PYD1598_SERIN_MASK & ~(0x00000001 << PYD1598_SHIFT_RESERVED2))

//Direct Link

//Note that this is to ensure the size of the type
#define PYD1598_DIRECT_LINK_MASK		(uint64_t)	0x0000008000000000
#define PYD1598_DIRECT_LINK_DATA_MASK	(uint64_t)	0x000000FFFFFFFFFF

#define PYD1598_OUT_OF_RANGE		(uint64_t)	PYD1598_DIRECT_LINK_MASK

#define PYD1598_ADC_COUNTS_SHIFT	25
#define PYD1598_ADC_COUNTS_MASK		(uint64_t) (((uint64_t)0x0000000000003FFF) << PYD1598_ADC_COUNTS_SHIFT)

#define PYD1598_CONFIG_SHIFT		0
#define PYD1598_CONFIG_MASK			(uint64_t)	0x01FFFFFF


/*****************************Functions declarations***************************/

/**********************************High Level**********************************/

void pyd1598_setup(pyd1598_sensor_t *sensor, pyd1598_config_t initial_config,
							pyd1598_hardware_interface_t serin_inteface,
							pyd1598_hardware_interface_t direct_link_inteface);


uint8_t pyd1598_check_isr_request(pyd1598_sensor_t sensor,
								pyd1598_motion_isr_status_t *wakeup_isr_status);

uint8_t pyd1598_read_wakeup_signal(pyd1598_sensor_t *sensor,
					volatile pyd1598_motion_isr_status_t *motion_isr_status);



void pyd1598_serin_setup(pyd1598_serin_t *serin,
							pyd1598_config_t initial_config,
							pyd1598_hardware_interface_t hardware_inteface);

void pyd1598_direct_link_setup(pyd1598_direct_link_t *direct_link,
							pyd1598_hardware_interface_t hardware_inteface);

//TODO: (high) deprecate this four functions:
pyd1598_readout_status_t pyd1598_direct_link_readout_fsm(uint64_t *data);
pyd1598_readout_status_t pyd1598_direct_link_interrupt_readout_fsm( uint64_t *data);
pyd1598_readout_status_t pyd1598_direct_link_forced_readout_fsm(uint64_t *data);
pyd1598_readout_status_t pyd1598_direct_link_wake_up_fsm(pyd1598_wakeup_conf_t conf, uint64_t *data);

//TODO: (high) test:
void pyd1598_dl_readout_fsm(pyd1598_direct_link_t *direct_link,
													pyd1598_op_mode_t op_mode);
void pyd1598_serin_send_datagram_fsm(pyd1598_serin_t *serin,
									pyd1598_hardware_interface_t data_link_pin);

//SERIN functions
void pyd1598_request_write(pyd1598_sensor_t *sensor);




void pyd1598_serin_set_zero_pulse(pyd1598_hardware_interface_t gpio);
void pyd1598_serin_set_one_pulse(pyd1598_hardware_interface_t gpio);
void pyd1598_serin_set_end_of_msg(pyd1598_hardware_interface_t gpio);





//Direct Link functions

void pyd1598_direct_link_set_as_output(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_set_as_input(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_set_as_interrupt_input(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_set_zero_pulse(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_set_one_pulse(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_set_negative_edge(pyd1598_hardware_interface_t gpio);
void pyd1598_direct_link_get_bit_value(pyd1598_hardware_interface_t gpio,
													GPIO_PinState *bit_value);
uint8_t pyd1598_direct_link_read_pin(pyd1598_hardware_interface_t gpio,
											uint64_t *buffer, uint32_t shift);


uint8_t pyd1598_direct_link_extract(uint32_t buffer, uint32_t *value,
										uint32_t mask, uint32_t shift);
uint8_t pyd1598_direct_link_get_adc_counts(pyd1598_direct_link_t *direct_link,
											uint16_t *adc_value,
											pyd1598_signal_source_t *source);

uint8_t pyd1598_direct_link_get_blind_time(pyd1598_direct_link_t *direct_link,
											pyd1598_blind_time_t *blind_time);
uint8_t pyd1598_direct_link_get_pulse_counter(pyd1598_direct_link_t *direct_link,
										pyd1598_pulse_counter_t *pulse_counter);

uint8_t pyd1598_direct_link_get_window_time(pyd1598_direct_link_t *direct_link,
											pyd1598_pulse_counter_t *window_time);

uint8_t pyd1598_direct_link_get_operation_mode(pyd1598_direct_link_t *direct_link,
											pyd1598_op_mode_t *operation_mode);

uint8_t pyd1598_direct_link_get_signal_source(pyd1598_direct_link_t *direct_link,
										pyd1598_signal_source_t *signal_source);

uint8_t pyd1598_direct_link_get_hpf_cutoff(pyd1598_direct_link_t *direct_link,
												pyd1598_hpf_cutoff_t *hpf_cutoff);

uint8_t pyd1598_direct_link_get_count_mode(pyd1598_direct_link_t *direct_link,
											pyd1598_count_mode_t *count_mode);

uint8_t pyd1598_direct_link_get_out_of_range(pyd1598_direct_link_t *direct_link,
												pyd1598_out_of_range_t *status);


uint8_t pyd1598_direct_link_get_config(pyd1598_direct_link_t *direct_link,
													uint32_t *conf_datagram);

uint8_t pyd1598_direct_link_get_threshold(pyd1598_direct_link_t *direct_link,
														uint8_t *threshold);



uint8_t pyd1598_serin_add_to_datagram(uint8_t data, uint32_t *datagram,
												uint32_t mask, uint8_t shift);

uint8_t pyd1598_serin_add_all_conf(pyd1598_config_t config,
													pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_threshold(uint8_t new_threshold,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_blind_time(uint8_t new_blind_time,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_pulse_counter(pyd1598_pulse_counter_t pulse_counter,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_window_time(pyd1598_window_time_t new_window_time,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_operation_mode(pyd1598_op_mode_t new_operation_mode,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_signal_source(pyd1598_signal_source_t signal_source,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_hpf_cutoff(pyd1598_hpf_cutoff_t new_hpf_cutoff,
														pyd1598_serin_t *serin);
uint8_t pyd1598_serin_add_count_mode(pyd1598_count_mode_t new_count_mode,
														pyd1598_serin_t *serin);

#ifdef __cplusplus
}
#endif
#endif /* INC_PYD15X8_H_ */
