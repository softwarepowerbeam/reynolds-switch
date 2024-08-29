


#include "BoardSupport/pyd15X8.h"

//TODO: (high) change the lower layer (hardware layer) to avoid the use of this
//include:
//#include "main.h"

//Information from:
//https://www.excelitas.com/product/pyd-1588-pyd-1598-low-power-digipyros








/**********************************High Level**********************************/
void pyd1598_setup(pyd1598_sensor_t *sensor, pyd1598_config_t initial_config,
							pyd1598_hardware_interface_t serin_inteface,
							pyd1598_hardware_interface_t direct_link_inteface)
{
	pyd1598_serin_setup(&(sensor->serin), initial_config, serin_inteface);
	pyd1598_direct_link_setup(&(sensor->direct_link), direct_link_inteface);



	sensor->current_config.blind_time = 0;
	sensor->current_config.count_mode = 0;
	sensor->current_config.hpf_cutoff = 0;
	sensor->current_config.op_mode = 0;
	sensor->current_config.pulse_counter = 0;
	sensor->current_config.signal_source = 0;
	sensor->current_config.threshold = 0;
	sensor->current_config.window_time = 0;





}

void pyd1598_serin_setup(pyd1598_serin_t *serin,
							pyd1598_config_t initial_config,
							pyd1598_hardware_interface_t hardware_inteface)
{
	serin->config = initial_config;


	pyd1598_serin_add_all_conf(serin->config, serin);

	serin->hardware_inteface = hardware_inteface;
	//This is incremented when data is send to SERIN pin
	serin->data_idx = 0;
	//This mask is shifted when data is send to SERIN pin.
	serin->mask = PYD1598_SERIN_BIT_24_MASK;
	serin->state = PYD1598_SERIN_IDLE;
	serin->start_fsm = false;




}

void pyd1598_direct_link_setup(pyd1598_direct_link_t *direct_link,
							pyd1598_hardware_interface_t hardware_inteface)
{
	//Hardware initializations
	direct_link->hardware_inteface = hardware_inteface;

	//Sensor variables initializations
	//This will be updated after the first comm
	direct_link->config.blind_time = 0;
	direct_link->config.count_mode = 0;
	direct_link->config.hpf_cutoff = 0;
	direct_link->config.op_mode = 0;
	direct_link->config.pulse_counter = 0;
	direct_link->config.signal_source = 0;
	direct_link->config.threshold = 0;
	direct_link->config.window_time = 0;

	direct_link->status = PYD1598_OUT_OF_RANGE_NORMAL;
	direct_link->source_value.pir_bpf = 0;
	direct_link->source_value.pir_lpf = 0;
	direct_link->source_value.temperature = 0;

	//Finite State Machine initializations
	direct_link->forced_state = PYD_STATE_FORCED_DRIVE_ONE;
	direct_link->datagram_bufffer = 0;
	direct_link->start_fsm = 0;
	direct_link->time_update_idx = 0;


//	if(direct_link->config.op_mode == PYD1598_FORCE_READOUT)
//	{
//		pyd1598_direct_link_set_as_input(direct_link->hardware_inteface);
//	}
//	else
//	{
//		pyd1598_direct_link_set_as_interrupt_input(direct_link->hardware_inteface);
//	}

}

/*---------------------------------Middleware FSM-----------------------------*/

/*_________________________________Direct link FSM____________________________*/


void pyd1598_dl_readout_fsm(pyd1598_direct_link_t *direct_link,
													pyd1598_op_mode_t op_mode)
{

	//Information variables
	uint64_t datagram_bufffer = direct_link->datagram_bufffer;

	//FSM variables
	pyd1598_wakeup_conf_t wakeup_mode = direct_link->wakeup_mode;
	pyd1598_readout_status_t readout_status = direct_link->readout_status;
	pyd1598_readout_state_t readout_state = direct_link->readout_state;
	uint8_t time_update_idx = direct_link->time_update_idx;
	bool start_fsm = direct_link->start_fsm;
	pyd1598_wakeup_isr_status_t wakeup_isr_status = direct_link->wakeup_isr_status;

	//Local variable
	uint32_t idx = 0;


	//TODO: (low) implement readout abort state

	switch(readout_state)
	{
		case PYD1598_READOUT_IDLE:

			//This state do nothing until start_fsm is activated outside the FSM
			if(start_fsm == true)
			{
				//From Table 5 and section 2.7 of the datasheet we know that the
				//time difference from this state and the next ones should be
				// 					120μs < t_{DS} < 150μs
				if(op_mode == PYD1598_FORCE_READOUT)
				{
					pyd1598_direct_link_set_as_output(direct_link->hardware_inteface);
					pyd1598_direct_link_set_one_pulse(direct_link->hardware_inteface);
					readout_state = PYD1598_READOUT_GET_MSG;

				}
				else if((op_mode == PYD1598_INTERRUPT_READOUT) ||
						(op_mode == PYD1598_WAKE_UP))
				{
					//TODO: (high) is it possible to set another flag if it
					//comes from an interrupt

					pyd1598_direct_link_set_as_output(direct_link->hardware_inteface);
					pyd1598_direct_link_set_negative_edge(direct_link->hardware_inteface);

					if((op_mode != PYD1598_INTERRUPT_READOUT) &&
						(wakeup_mode == READOUT_WAKEUP_NO_READ))
					{
						//since in Wake-up mode read is optional:
						readout_state = PYD1598_READOUT_END_COM;
					}
					else
					{
						readout_state = PYD1598_READOUT_GET_MSG;
					}
				}
				else
				{
					//TODO: (low) define an error handler
				}

				start_fsm = false;
				readout_status = READOUT_BUSY;

			}

			break;
		case PYD1598_READOUT_GET_MSG:

			//TODO: (HIGH) remember to protect from interrupts!!!!!!!!!!!!!

			datagram_bufffer = 0;

			//Proceeding with the Readout:
			for(idx = 0; idx < 40; idx++)
			{
				pyd1598_direct_link_set_one_pulse(direct_link->hardware_inteface);
				pyd1598_direct_link_set_as_input(direct_link->hardware_inteface);

				//TODO: (low) Test without using a function call so the pin bit
				// can be faster.
				pyd1598_direct_link_read_pin(direct_link->hardware_inteface,
													&datagram_bufffer, idx);

				pyd1598_direct_link_set_negative_edge(direct_link->hardware_inteface);
				pyd1598_direct_link_set_as_output(direct_link->hardware_inteface);

			}

			//host controller must force DIRECT LINK pin to LOW for at least
			//t_{UP} >  1250μs and subsequently release DIRECT LINK (High Z)


			time_update_idx = 0;

			readout_status = READOUT_BUSY;
			readout_state = PYD1598_READOUT_SIGNAL_DOWN;

			break;
		case PYD1598_READOUT_SIGNAL_DOWN:
			pyd1598_direct_link_set_as_output(direct_link->hardware_inteface);
			pyd1598_direct_link_set_negative_edge(direct_link->hardware_inteface);

			if( (op_mode == PYD1598_INTERRUPT_READOUT) ||
				(op_mode == PYD1598_WAKE_UP))
			{
				//since these op modes require an external interrupt
				pyd1598_direct_link_set_as_interrupt_input(direct_link->hardware_inteface);
				readout_status = READOUT_READY;
				readout_state = PYD1598_READOUT_IDLE;
				time_update_idx = 0;

				if(op_mode == PYD1598_WAKE_UP)
				{
					wakeup_isr_status = PYD1598_WAKEUP_ISR_UNATTENDED;
				}


			}
			else
			{
				readout_status = READOUT_BUSY;
				readout_state = PYD1598_READOUT_UPDATE;
			}



		case PYD1598_READOUT_UPDATE:
			// considering that interrupt time is selected according with
			// 					120μs < t_{DS} < 150μs,
			//and since
			//					t_{UP} >  1250μs
			//this state should remain at least 1250/120 = 10.42 -> 11 cycles.

			time_update_idx++;

			//TODO: (low) create a mechanism to configure the number of cycles
			if(time_update_idx >= 11)
			{
				time_update_idx = 0;
				readout_status = READOUT_BUSY;
				readout_state = PYD1598_READOUT_END_COM;
			}

			break;

		case PYD1598_READOUT_END_COM:
			//TODO: (low) check if this conditional is required since it is done
			//in PYD1598_READOUT_SIGNAL_DOWN.

			//release DIRECT LINK (High Z)
			if(op_mode == PYD1598_FORCE_READOUT)
			{
				pyd1598_direct_link_set_as_input(direct_link->hardware_inteface);
			}
			else if((op_mode == PYD1598_INTERRUPT_READOUT) ||
					(op_mode == PYD1598_WAKE_UP))
			{
				//since this op modes require an external interrupt
				pyd1598_direct_link_set_as_interrupt_input(direct_link->hardware_inteface);

				if(op_mode == PYD1598_WAKE_UP)
				{
					wakeup_isr_status = PYD1598_WAKEUP_ISR_UNATTENDED;
				}

			}
			readout_status = READOUT_READY;
			readout_state = PYD1598_READOUT_IDLE;

		default:
			readout_state = PYD1598_READOUT_IDLE;
			readout_status = READOUT_READY;
			break;
	}

	direct_link->readout_state = readout_state;
	direct_link->readout_status = readout_status;
	direct_link->time_update_idx = time_update_idx;
	direct_link->start_fsm = start_fsm;
	//Saving datagram to be read outside the ISR:
	direct_link->datagram_bufffer = datagram_bufffer;
	direct_link->wakeup_isr_status = wakeup_isr_status;


}



/*____________________________________SERIN FSM_______________________________*/

void pyd1598_request_write(pyd1598_sensor_t *sensor)
{
	sensor->serin.start_fsm = true;
}


void pyd1598_serin_send_datagram_fsm(pyd1598_serin_t *serin,
									pyd1598_hardware_interface_t data_link_pin)
{
	//Declaring the FSM variables and define them to protect the serin variables
	//from any outside change

	pyd1598_serin_state_t state = serin->state;//Define if it requires to be static
	uint32_t data_idx = serin->data_idx;
	bool start_fsm = serin->start_fsm;
	pyd1598_config_t config = serin->config;
	uint32_t mask = serin->mask;
	uint32_t conf_datagram = serin->conf_datagram;

	pyd1598_hardware_interface_t output = serin->hardware_inteface;

	switch(state)
	{
		case PYD1598_SERIN_IDLE:
			if(start_fsm == true)
			{
				pyd1598_direct_link_set_as_output(data_link_pin);
				pyd1598_direct_link_set_negative_edge(data_link_pin);
				state = PYD1598_SERIN_SETUP_FOR_MESSAGE;
				start_fsm = false;
			}
			break;
		case PYD1598_SERIN_SETUP_FOR_MESSAGE:

			data_idx = 0;
//			pyd1598_direct_link_set_as_output();
//			pyd1598_direct_link_set_negative_edge();

			mask = PYD1598_SERIN_BIT_24_MASK;
			state = PYD1598_SERIN_SEND_MSG;

			break;
		case PYD1598_SERIN_SEND_MSG:
			// program the timer interrupt considering: t_{SHD} = 80-150μs
			if((conf_datagram & mask) == 0)
			{
				pyd1598_serin_set_zero_pulse(output);
			}
			else
			{
				pyd1598_serin_set_one_pulse(output);
			}

			mask >>= 1;

//			if(data_idx >= (PYD1598_SERIN_MSG_LONG+1))
			if(mask == 0)
			{
				state = PYD1598_SERIN_SEND_DOWN;
			}
			else
			{
				data_idx++;
			}

			break;
		case PYD1598_SERIN_SEND_DOWN:
			pyd1598_direct_link_set_as_output(data_link_pin);
			pyd1598_direct_link_set_negative_edge(data_link_pin);
			state = PYD1598_SERIN_WAIT_LOAD_TIME;

			break;

		case PYD1598_SERIN_WAIT_LOAD_TIME:
			//Data Load Time t_{SLT} > 650μs. i.e. 8.125 to 4.333 times
			//(9 times for a 80μs interrupt or 4 times for a 150μs interrupt)
			if(data_idx >= PYD1598_SERIN_COUNT_TSLT)
			{
				state = PYD1598_SERIN_END_COM;
			}
			else
			{
				data_idx++;
			}
			break;
		case PYD1598_SERIN_END_COM:

			if(config.op_mode == PYD1598_FORCE_READOUT)
			{
				pyd1598_direct_link_set_as_input(data_link_pin);
			}
			else if((config.op_mode == PYD1598_INTERRUPT_READOUT) ||
					(config.op_mode == PYD1598_WAKE_UP) )
			{
				pyd1598_direct_link_set_as_interrupt_input(data_link_pin);
			}
			state = PYD1598_SERIN_IDLE;
			break;
		default:
			state = PYD1598_SERIN_IDLE;
			break;

	}

	//This are the FSM variables that are changed in this function.
	serin->state = state;
	serin->data_idx = data_idx;
	serin->start_fsm = false;
	serin->mask = mask;
	serin->start_fsm = start_fsm;

}

/*******************************Middleware Level*******************************/

/*---------------------------Middleware Level functions-----------------------*/
/*____________________________Direct link functions___________________________*/

uint8_t pyd1598_check_isr_request(pyd1598_sensor_t sensor,
								pyd1598_motion_isr_status_t *motion_isr_status)
{
	*motion_isr_status = sensor.motion_sensed;
	return 0;
}

//Use this in the main loop
uint8_t pyd1598_read_wakeup_signal(pyd1598_sensor_t *sensor,
								pyd1598_motion_isr_status_t *motion_isr_status)
{

	if( sensor->direct_link.wakeup_isr_status == PYD1598_WAKEUP_ISR_UNATTENDED)
	{
		sensor->direct_link.wakeup_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;
		*motion_isr_status = PYD1598_MOTION_ISR_UNATTENDED;
		sensor->motion_sensed = PYD1598_MOTION_ISR_UNATTENDED;
	}

	return 0;
}






uint8_t pyd1598_direct_link_get_out_of_range(pyd1598_direct_link_t *direct_link,
												pyd1598_out_of_range_t *status)
{

	if((direct_link->datagram_bufffer & PYD1598_DIRECT_LINK_MASK) == 0)
	{
		*status = PYD1598_OUT_OF_RANGE_RESET;
	}
	else
	{
		*status = PYD1598_OUT_OF_RANGE_NORMAL;
	}

	direct_link->status = *status;

	return 0;
}


uint8_t pyd1598_direct_link_get_adc_counts(pyd1598_direct_link_t *direct_link,
												uint16_t *adc_value,
												pyd1598_signal_source_t *source)
{
	uint32_t counts = 0;

	counts = ((uint64_t) 	(direct_link->datagram_bufffer &
							PYD1598_ADC_COUNTS_MASK) >>
							PYD1598_ADC_COUNTS_SHIFT);


	pyd1598_direct_link_get_config(direct_link, &direct_link->datagram_config);

	pyd1598_direct_link_get_signal_source(direct_link, source);

	switch(*source)
	{
		case PYD1598_SOURCE_PIR_BFP:
			direct_link->source_value.pir_bpf = (uint16_t) counts;
			break;
		case PYD1598_SOURCE_PIR_LPF:
			direct_link->source_value.pir_lpf = (uint16_t) counts;
			break;
		case PYD1598_SOURCE_RESERVED:
			//do nothing
			break;
		case PYD1598_SOURCE_TEMP:
			direct_link->source_value.temperature = (uint16_t) counts;
			break;
		default:
			//do nothing
			break;
	}

	*adc_value = (uint16_t) counts;

	return 0;
}

uint8_t pyd1598_direct_link_get_config(pyd1598_direct_link_t *direct_link,
														uint32_t *conf_datagram)
{

	*conf_datagram = (uint32_t) (	direct_link->datagram_bufffer &
									PYD1598_CONFIG_MASK);

	direct_link->datagram_config = *conf_datagram;
	return 0;
}

uint8_t pyd1598_direct_link_get_threshold(pyd1598_direct_link_t *direct_link,
															uint8_t *threshold)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_THRESHOLD;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
													PYD1598_SHIFT_THRESHOLD);

	*threshold = (uint8_t) value;
	direct_link->config.threshold = (uint8_t) value;

	return 0;
}


uint8_t pyd1598_direct_link_get_blind_time(pyd1598_direct_link_t *direct_link,
											pyd1598_blind_time_t *blind_time)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_BLIND_TIME;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
													PYD1598_SHIFT_BLIND_TIME);

	*blind_time = (uint8_t) value;
	direct_link->config.blind_time = (uint8_t) value;
	return 0;
}

uint8_t pyd1598_direct_link_get_pulse_counter(pyd1598_direct_link_t *direct_link,
										pyd1598_pulse_counter_t *pulse_counter)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_PULSE_COUNTER;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
											PYD1598_SHIFT_PULSE_COUNTER);

	*pulse_counter = (uint8_t) value;
	direct_link->config.pulse_counter = (uint8_t) value;
	return 0;
}

uint8_t pyd1598_direct_link_get_window_time(pyd1598_direct_link_t *direct_link,
										pyd1598_pulse_counter_t *window_time)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_WINDOW_TIME;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
												PYD1598_SHIFT_WINDOW_TIME);
	*window_time = (uint8_t) value;
	direct_link->config.window_time = (uint8_t) value;

	return 0;
}


uint8_t pyd1598_direct_link_get_operation_mode(pyd1598_direct_link_t *direct_link,
												pyd1598_op_mode_t *operation_mode)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_OPERATION_MODES;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
												PYD1598_SHIFT_OPERATION_MODES);

	switch(value)
	{
		case 0:
			*operation_mode = PYD1598_FORCE_READOUT;
			break;
		case 1:
			*operation_mode = PYD1598_INTERRUPT_READOUT;
			break;
		case 2:
			*operation_mode = PYD1598_WAKE_UP;
			break;
		case 3:
			*operation_mode = PYD1598_RESERVED;
			break;
		default:
			//do nothing
			break;
	}
	direct_link->config.op_mode = *operation_mode;

	return 0;

}

uint8_t pyd1598_direct_link_get_signal_source(pyd1598_direct_link_t *direct_link,
										pyd1598_signal_source_t *signal_source)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_SIGNAL_SOURCE;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
												PYD1598_SHIFT_SIGNAL_SOURCE);

	switch(value)
	{
		case 0:
			*signal_source = PYD1598_SOURCE_PIR_BFP;
			break;
		case 1:
			*signal_source = PYD1598_SOURCE_PIR_LPF;
			break;
		case 2:
			*signal_source = PYD1598_SOURCE_RESERVED;
			break;
		case 3:
			*signal_source = PYD1598_SOURCE_TEMP;
			break;
		default:
			//do nothing
			break;
	}
	direct_link->config.signal_source = *signal_source;
	return 0;
}

uint8_t pyd1598_direct_link_get_hpf_cutoff(pyd1598_direct_link_t *direct_link,
												pyd1598_hpf_cutoff_t *hpf_cutoff)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_HPF_CUTOFF;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
													PYD1598_SHIFT_HPF_CUTOFF);

	switch(value)
	{
		case 0:
			*hpf_cutoff = PYD1598_HPF_0_4HZ;
			break;
		case 1:
			*hpf_cutoff = PYD1598_HPF_0_2HZ;
			break;
		default:
			//do nothing
			break;
	}
	direct_link->config.hpf_cutoff = *hpf_cutoff;
	return 0;
}

uint8_t pyd1598_direct_link_get_count_mode(pyd1598_direct_link_t *direct_link,
											pyd1598_count_mode_t *count_mode)
{
	uint32_t value = 0;
	uint32_t mask = PYD1598_SERIN_MASK & ~PYD1598_MASK_COUNT_MODE;

	pyd1598_direct_link_extract(direct_link->datagram_config, &value, mask,
											PYD1598_SHIFT_COUNT_MODE);

	switch(value)
	{
		case 0:
			*count_mode = PYD1598_COUNT_MODE_NO_BPF_SIGN_CHANGE;
			break;
		case 1:
			*count_mode = PYD1598_COUNT_MODE_BPF_SIGN_CHANGE;
			break;
		default:
			//do nothing
			break;
	}
	direct_link->config.count_mode = *count_mode;
	return 0;
}

uint8_t pyd1598_direct_link_extract(uint32_t buffer, uint32_t *value,
										uint32_t mask, uint32_t shift)
{
	*value = (buffer & mask) >> shift;
	return 0;
}


/*______________________________SERIN functions_______________________________*/


uint8_t pyd1598_serin_add_all_conf(pyd1598_config_t config,
													pyd1598_serin_t *serin)
{
	uint8_t status = 0;
	//TODO: (low) set error handler
	pyd1598_serin_add_threshold(config.threshold, serin);
	pyd1598_serin_add_blind_time(config.blind_time, serin);
	pyd1598_serin_add_pulse_counter(config.pulse_counter, serin);
	pyd1598_serin_add_window_time(config.window_time, serin);
	pyd1598_serin_add_operation_mode(config.op_mode, serin);
	pyd1598_serin_add_signal_source(config.signal_source, serin);
	pyd1598_serin_add_hpf_cutoff(config.hpf_cutoff, serin);
	pyd1598_serin_add_count_mode(config.count_mode, serin);
	return status;
}


//uint8_t pyd1598_serin_add_threshold(uint8_t new_threshold, uint32_t *datagram)
uint8_t pyd1598_serin_add_threshold(uint8_t new_threshold,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	serin->config.threshold = new_threshold;
	status = pyd1598_serin_add_to_datagram(new_threshold, &serin->conf_datagram,
											PYD1598_MASK_THRESHOLD,
											PYD1598_SHIFT_THRESHOLD);
	return status;
}

// blind_time = 0.5 s + [Reg Val] · 0.5 s
//Reg Val is 4 bits ...
//max_value 8sec =8000ms
//min value 0.5sec =500ms
uint8_t pyd1598_serin_add_blind_time(pyd1598_blind_time_t new_blind_time,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	new_blind_time &= 0x0F;
	serin->config.blind_time = new_blind_time;
	status = pyd1598_serin_add_to_datagram(new_blind_time,
											&serin->conf_datagram,
											PYD1598_MASK_BLIND_TIME,
											PYD1598_SHIFT_BLIND_TIME);
	return status;
}

uint8_t pyd1598_serin_add_pulse_counter(pyd1598_pulse_counter_t pulse_counter,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	pulse_counter &= 0x03;
	serin->config.pulse_counter = pulse_counter;
	status = pyd1598_serin_add_to_datagram(pulse_counter, &serin->conf_datagram,
											PYD1598_MASK_PULSE_COUNTER,
											PYD1598_SHIFT_PULSE_COUNTER);
	return status;
}
uint8_t pyd1598_serin_add_window_time(pyd1598_window_time_t window_time,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	window_time &= 0x03;
	serin->config.window_time = window_time;
	status = pyd1598_serin_add_to_datagram(window_time, &serin->conf_datagram,
										PYD1598_MASK_WINDOW_TIME,
										PYD1598_SHIFT_WINDOW_TIME);
	return status;
}
uint8_t pyd1598_serin_add_operation_mode(pyd1598_op_mode_t new_op_mode,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;
	serin->config.op_mode = new_op_mode;
	status = pyd1598_serin_add_to_datagram((uint8_t) new_op_mode,
											&serin->conf_datagram,
											PYD1598_MASK_OPERATION_MODES,
											PYD1598_SHIFT_OPERATION_MODES);
	return status;
}
uint8_t pyd1598_serin_add_signal_source(pyd1598_signal_source_t signal_source,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	signal_source &= 0x03;
	serin->config.signal_source = signal_source;
	status = pyd1598_serin_add_to_datagram((uint8_t) signal_source,
												&serin->conf_datagram,
												PYD1598_MASK_SIGNAL_SOURCE,
												PYD1598_SHIFT_SIGNAL_SOURCE);
	return status;
}

uint8_t pyd1598_serin_add_hpf_cutoff(pyd1598_hpf_cutoff_t new_hpf_cutoff,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	new_hpf_cutoff &= 0x03;
	serin->config.hpf_cutoff = new_hpf_cutoff;
	status = pyd1598_serin_add_to_datagram((uint8_t) new_hpf_cutoff,
											&serin->conf_datagram,
											PYD1598_MASK_HPF_CUTOFF,
											PYD1598_SHIFT_HPF_CUTOFF);
	return status;
}

uint8_t pyd1598_serin_add_count_mode(pyd1598_count_mode_t new_count_mode,
														pyd1598_serin_t *serin)
{
	uint8_t status = 0;

	new_count_mode &= 0x03;
	serin->config.count_mode = new_count_mode;
	status = pyd1598_serin_add_to_datagram((uint8_t) new_count_mode,
											&serin->conf_datagram,
											PYD1598_MASK_COUNT_MODE,
											PYD1598_SHIFT_COUNT_MODE);
	return status;
}

uint8_t pyd1598_serin_add_to_datagram(uint8_t data, uint32_t *datagram,
												uint32_t mask, uint8_t shift)
{
	uint32_t old_config = 0;
	uint32_t new_config = 0;

	//generating copy of the datagram to handle the changes
	//(protecting from possible changes due to interrupts)
	old_config = *datagram;

	//Clear section of the datagram copy to set the new data
	old_config &= mask;

	//Shifting data to the position of the datagram copy
	new_config |= (((uint32_t) data) << shift);

	//Adding data to the datagram copy
	new_config |= old_config;

	//substitute the values of the datagram with the new configuration
	*datagram = new_config;

	return 0;
}

/*********************************Hardware Layer*******************************/

//Information from:
//https://www.excelitas.com/product/pyd-1588-pyd-1598-low-power-digipyros

//TODO: (high) change this to decouple from the main.h header.
void pyd1598_serin_set_zero_pulse(pyd1598_hardware_interface_t gpio)
{
	//This should takes less than t_{DL}=200-2000ns
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
}

void pyd1598_serin_set_one_pulse(pyd1598_hardware_interface_t gpio)
{
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_SET);
}

void pyd1598_serin_set_end_of_msg(pyd1598_hardware_interface_t gpio)
{
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
}

void pyd1598_direct_link_set_as_output(pyd1598_hardware_interface_t gpio)
{
	HAL_DIRECT_LINK_conf_as_output(gpio.port, gpio.pin, gpio.irq_type);
}

void pyd1598_direct_link_set_as_input(pyd1598_hardware_interface_t gpio)
{
	HAL_DIRECT_LINK_conf_as_input(gpio.port, gpio.pin, gpio.irq_type);
}

void pyd1598_direct_link_set_as_interrupt_input(pyd1598_hardware_interface_t gpio)
{
	HAL_DIRECT_LINK_conf_as_interrupt_input(gpio.port, gpio.pin, gpio.irq_type);
}

void pyd1598_direct_link_set_zero_pulse(pyd1598_hardware_interface_t gpio)
{
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
}
void pyd1598_direct_link_set_one_pulse(pyd1598_hardware_interface_t gpio)
{
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_SET);
}
void pyd1598_direct_link_set_negative_edge(pyd1598_hardware_interface_t gpio)
{
	HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
}
void pyd1598_direct_link_get_bit_value(pyd1598_hardware_interface_t gpio,
											GPIO_PinState *bit_value)
{
	 *bit_value = HAL_GPIO_ReadPin(gpio.port, gpio.pin);
}


uint8_t pyd1598_direct_link_read_pin(pyd1598_hardware_interface_t gpio,
											uint64_t *buffer, uint32_t shift)
{
	static uint64_t readout_mask = (uint64_t) PYD1598_DIRECT_LINK_MASK;
	GPIO_PinState pin_value = GPIO_PIN_RESET;

	pin_value = HAL_GPIO_ReadPin(gpio.port, gpio.pin);

	if(pin_value == GPIO_PIN_SET)
	{
		*buffer  |= readout_mask;
	}
	else if(pin_value == GPIO_PIN_RESET)
	{
		*buffer  &= ~readout_mask;
	}
	else
	{
		__NOP();
	}

	readout_mask = (readout_mask >> 1);

	if(readout_mask == ((uint64_t) 0))
	{
		readout_mask  = (uint64_t) PYD1598_DIRECT_LINK_MASK;
	}
	return 0;

}
