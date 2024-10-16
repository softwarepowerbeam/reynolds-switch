/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>

#define DEBUG_REYNOLDS

//#define LIGHT_AS_ACTUATOR
//#define DEBUG_UART_REYNOLDS



#ifdef DEBUG_UART_REYNOLDS
#include <stdio.h>
#endif //DEBUG_UART_REYNOLDS

#include "BoardSupport/pyd15X8.h"
#include "BoardSupport/button.h"
#include "BoardSupport/lights.h"
#include "BoardSupport/led_indicator.h"
#include "BoardSupport/switch_selector.h"
#include "BoardSupport/relay_l114fl_drv8210.h"


#include "Middleware/deadline_timer.h"
#include "Middleware/output_ctrl.h"
#include "Middleware/NuTone.h"

#include "Application/nutone_app.h"

#include "Application/motion_lights_driver.h"
#include "app.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

//CONFIGURE THE BASIC MOTION PARAMETERS HERE:<---------------------------------------------------------------------------|

//#define TEST_TIMEOUT

/******************************************************************************/
/******************************************************************************/
/***************************CONFIG SECTION 1***********************************/
/******************************************************************************/
/******************************************************************************/
#define DEBUG_MEMORY//<<----This is inverted, if you want to enable all the modes available UNCOMMENT THIS (LOGIC NAME IS WRONG)

#define PRODUCTION_PARAM //<<---------Comment for fast tests

#ifndef PRODUCTION_PARAM//Time parameters for the app
#define 	FAST_TEST
#endif //FAST_TEST

#define MINUTES_2_MILI_SECONDS	60000
#define HOURS_2_MILI_SECONDS	60 * MINUTES_2_MILI_SECONDS

#ifdef PRODUCTION_PARAM
//Office Mode:
//-------------------------------------------------
#define OFFICE_MOTION_SENSOR_DETECTION_THRESHOLD		6		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t OFFICE_MOTION_SENSOR_WINDOW = PYD1598_WT_8_SEC;

#define OFFICE_LAMP1_ON_TIME_MS				10 * MINUTES_2_MILI_SECONDS	//!<Waiting period of Lamp 1 illumination in milisec
#define OFFICE_LAMP2_ON_TIME_MS				10 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp 2 illumination in milisec
#define OFFICE_LAMP_UV_SAFETY_TIME_MS		900 	//!<Waiting period before turning on UV light in milisec
#define OFFICE_LAMP_UV_ON_TIME_MS			20 * MINUTES_2_MILI_SECONDS	//!<Waiting period of Lamp UV illumination in milisec
#ifdef TEST_TIMEOUT
#define OFFICE_LAMP_UV_TIMEOUT_MS			30000
#else	//TEST_TIMEOUT
#define OFFICE_LAMP_UV_TIMEOUT_MS			10000						//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.
#endif	//TEST_TIMEOUT

//Residential mode:
//-------------------------------------------------
#define RESIDENTIAL_MOTION_SENSOR_DETECTION_THRESHOLD	7		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t RESIDENTIAL_MOTION_SENSOR_WINDOW = PYD1598_WT_8_SEC;

#define RESIDENTIAL_LAMP1_ON_TIME_MS		10 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp 1 illumination in milisec
#define RESIDENTIAL_LAMP2_ON_TIME_MS		0							//!<Waiting period of Lamp 2 illumination in milisec
#define RESIDENTIAL_LAMP_UV_SAFETY_TIME_MS	900 	//!<Waiting period before turning on UV light in milisec
#define RESIDENTIAL_LAMP_UV_ON_TIME_MS		3 * HOURS_2_MILI_SECONDS 	//!<Waiting period of Lamp UV illumination in milisec
#ifdef TEST_TIMEOUT
#define RESIDENTIAL_LAMP_UV_TIMEOUT_MS		30000
#else	//TEST_TIMEOUT
#define RESIDENTIAL_LAMP_UV_TIMEOUT_MS		10000						//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.
#endif	//TEST_TIMEOUT


//Broan NuTone SurfaceShield mode:
//-------------------------------------------------
#define SURFACESHIELD_MOTION_SENSOR_DETECTION_THRESHOLD	7		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t SURFACESHIELD_MOTION_SENSOR_WINDOW = PYD1598_WT_8_SEC;

#define SURFACESHIELD_LAMP1_ON_TIME_MS		10 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp 1 illumination in milisec
#define SURFACESHIELD_LAMP2_ON_TIME_MS		0							//!<Waiting period of Lamp 2 illumination in milisec
#define SURFACESHIELD_LAMP_UV_SAFETY_TIME_MS	900 	//!<Waiting period before turning on UV light in milisec
#define SURFACESHIELD_LAMP_UV_ON_TIME_MS		3 * HOURS_2_MILI_SECONDS 	//!<Waiting period of Lamp UV illumination in milisec

#define SURFACESHIELD_LAMP_UV_TIMEOUT_MS		10000	//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.


#endif //FAST_TEST





#ifdef FAST_TEST

//Office Mode:
#define OFFICE_MOTION_SENSOR_DETECTION_THRESHOLD		60		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t OFFICE_MOTION_SENSOR_WINDOW = PYD1598_WT_2_SEC;

#define OFFICE_LAMP1_ON_TIME_MS				1 * MINUTES_2_MILI_SECONDS	//!<Waiting period of Lamp 1 illumination in milisec
#define OFFICE_LAMP2_ON_TIME_MS				1 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp 2 illumination in milisec
#define OFFICE_LAMP_UV_SAFETY_TIME_MS		900 	//!<Waiting period before turning on UV light in milisec
#define OFFICE_LAMP_UV_ON_TIME_MS			2 * MINUTES_2_MILI_SECONDS	//!<Waiting period of Lamp UV illumination in milisec
#ifdef TEST_TIMEOUT
#define OFFICE_LAMP_UV_TIMEOUT_MS			30000
#else	//TEST_TIMEOUT
#define OFFICE_LAMP_UV_TIMEOUT_MS			10000						//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.
#endif	//TEST_TIMEOUT

//Residential mode:
#define RESIDENTIAL_MOTION_SENSOR_DETECTION_THRESHOLD	60		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t RESIDENTIAL_MOTION_SENSOR_WINDOW = PYD1598_WT_8_SEC;

#define RESIDENTIAL_LAMP1_ON_TIME_MS		10000//1 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp 1 illumination in milisec
#define RESIDENTIAL_LAMP2_ON_TIME_MS		0							//!<Waiting period of Lamp 2 illumination in milisec
#define RESIDENTIAL_LAMP_UV_SAFETY_TIME_MS	900 	//!<Waiting period before turning on UV light in milisec
#define RESIDENTIAL_LAMP_UV_ON_TIME_MS		10000//3 * MINUTES_2_MILI_SECONDS 	//!<Waiting period of Lamp UV illumination in milisec
#ifdef TEST_TIMEOUT
#define RESIDENTIAL_LAMP_UV_TIMEOUT_MS		30000
#else	//TEST_TIMEOUT
#define RESIDENTIAL_LAMP_UV_TIMEOUT_MS		5000						//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.
#endif	//TEST_TIMEOUT


//Broan NuTone SurfaceShield mode:
//-------------------------------------------------
#define SURFACESHIELD_MOTION_SENSOR_DETECTION_THRESHOLD	60		//!< Range 0-255 (0 more sensitive -255 less sensitive)
const pyd1598_window_time_t SURFACESHIELD_MOTION_SENSOR_WINDOW = PYD1598_WT_8_SEC;

#define SURFACESHIELD_LAMP1_ON_TIME_MS		10000 	//!<Waiting period of Lamp 1 illumination in milisec
#define SURFACESHIELD_LAMP2_ON_TIME_MS		0							//!<Waiting period of Lamp 2 illumination in milisec
#define SURFACESHIELD_LAMP_UV_SAFETY_TIME_MS	900 	//!<Waiting period before turning on UV light in milisec
#define SURFACESHIELD_LAMP_UV_ON_TIME_MS		10000 	//!<Waiting period of Lamp UV illumination in milisec
#define SURFACESHIELD_LAMP_UV_TIMEOUT_MS		5000						//!<Waiting period for timeout. If motion is detected and UV button had been pressed finishes process automatically.

#endif //FAST_TEST


/******************************************************************************/
/******************************************************************************/
/***************************CONFIG SECTION ENDS********************************/
/******************************************************************************/
/******************************************************************************/


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim17;

#ifdef DEBUG_UART_REYNOLDS
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#endif //DEBUG_UART_REYNOLDS

#ifdef DEBUG_REYNOLDS
uint8_t myflag = 0;
#endif //DEBUG_REYNOLDS


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/******************************************************************************/

#ifdef DEBUG_UART_REYNOLDS
//Adding printf
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif //DEBUG_UART_REYNOLDS

/****************************PYD 1598 routines*********************************/

//App routines:
void motion_light_control_fsm(relay_t *light,
								button_t *button,
								pyd1598_sensor_t *motion_sensor,
								deadline_timer_t *deadline_timer,
								motion_light_state_t *fsm_state,
								motion_sensed_t *motion_sensed);
void motion_light_uv_control_fsm(relay_t *light_uv,
									button_t *button_uv,
									pyd1598_sensor_t *motion_sensor,
									deadline_timer_t *deadline_timer,
									deadline_timer_t *deadline_safe_timer,
									motion_light_uv_state_t *fsm_state,
									motion_sensed_t *motion_sensed,
									motion_light_uv_abort_t *uv_abort,
									led_signal_t *signal);
void motion_uv_ctrl_wait_fsm(relay_t *light_uv,
								button_t *button_uv,
								pyd1598_sensor_t *motion_sensor,
								deadline_timer_t *deadline_timer,
								deadline_timer_t *deadline_safe_timer,
								deadline_timer_t *deadline_timeout,
								deadline_timer_t *deadline_wait_timeout,
								motion_light_uv_state_t *fsm_state,
								motion_sensed_t *motion_sensed,
								motion_light_uv_abort_t *uv_abort,
								motion_uv_wait_signal_t wait,
								led_signal_t *signal);

void direct_light_control_fsm(relay_t *light,
								button_t *button,
								deadline_timer_t *deadline_timer,
								motion_light_no_motion_state_t *fsm_state);




void events_detection(pyd1598_sensor_t *motion,
							deadline_timer_t *deadline_events,
							button_t *button_lamp_1, button_t *button_lamp_2,
							button_t *button_lamp_uv,
							motion_sensed_t *motion_light_1,
							motion_sensed_t *motion_light_2,
							motion_sensed_t *motion_uv,
							motion_light_uv_abort_t *abort_signal_uv);

void events_detection_motion_in_one_lamp(pyd1598_sensor_t *motion,
							deadline_timer_t *deadline_events,
							button_t *button_lamp_1,
							button_t *button_lamp_2,
							button_t *button_lamp_uv,
							motion_sensed_t *motion_light_1,
							motion_sensed_t *motion_uv,
							motion_light_uv_abort_t *abort_signal_uv);

void events_detection_uv_waits(pyd1598_sensor_t *motion,
							deadline_timer_t *deadline_events,
							button_t *button_lamp_1, button_t *button_lamp_2,
							button_t *button_lamp_uv,
							motion_sensed_t *motion_light_1,
							motion_sensed_t *motion_light_2,
							motion_sensed_t *motion_uv,
							motion_light_uv_abort_t *abort_signal_uv);


void discreate_actuator(light_t *actuator, deadline_timer_t *deadline_timer);


void sense_button_event(deadline_timer_t *deadline_events, button_t *button);

/******************************************************************************/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/******************************************************************************/

#ifdef DEBUG_REYNOLDS

#endif //DEBUG_REYNOLDS

//TODO: (low) Change to main process
//Testing upper layer:


//Motion Sensor declarations
pyd1598_sensor_t motion_sensor;
pyd1598_config_t motion_initial_conf;


//General Clock
timer_clock_t deadline;

//Lights declarations
deadline_timer_t deadline_timer_light_1;
deadline_timer_t deadline_timer_light_2;
deadline_timer_t deadline_timer_uv;

//General Clock
timer_clock_t general_clock;

//Buttons declarations
deadline_timer_t deadline_buttons;
timer_clock_t timer_buttons;//Change this to main
button_t button_light_1;
button_t button_light_2;
button_t button_uv;

motion_sensed_t motion_sensed_light_1;
motion_sensed_t motion_sensed_light_2;
motion_sensed_t motion_sensed_uv;

//LED indicator
deadline_timer_t deadline_led_indicator;//TODO: (High) deprecate this variable

//Application declarations
deadline_timer_t deadline_motion_light_1;
timer_clock_t timer_motion_light_1;//Change this to main
motion_light_state_t light_1_state;//Change this to main

deadline_timer_t deadline_motion_light_2;
timer_clock_t timer_motion_light_2;//Change this to main
motion_light_state_t light_2_state;//Change this to main
motion_light_no_motion_state_t light_2_no_motion_state;//Change this to main

//UV motion timers
deadline_timer_t deadline_motion_uv;
timer_clock_t timer_motion_uv;//Change this to main

deadline_timer_t deadline_motion_uv_safe;
timer_clock_t timer_motion_uv_safe;//Change this to main

deadline_timer_t deadline_motion_uv_timeout;//Change this to main
timer_clock_t timer_motion_uv_timeout;

deadline_timer_t deadline_uv_wait_timeout;//Change this to main
timer_clock_t timer_uv_wait_timeout;

motion_light_uv_state_t uv_state;//Change this to main

motion_light_uv_abort_t abort_uv = MOTION_ABORT_FALSE;
motion_uv_wait_signal_t wait = MOTION_UV_WAIT_FALSE;


//NuTone timers
deadline_timer_t ctrl_timer;
deadline_timer_t vyv_timeoff;
nutone_app_t nutone_app_hand;


/******************************************************************************/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM14_Init();
  MX_TIM3_Init();
  MX_TIM17_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  /**************************************************************************/
  general_clock.counts = 0;
  general_clock.msec = 0;




  //Configure PYD 159:
//  HAL_DIRECT_LINK_conf_as_interrupt_input();


  //DIP SWITCH SETUP
  //----------------------------------------------------------------------------
  switch_selector_t switch_selector;
  switch_selector_gpio_t switch_selector_gpio[2];//We can use malloc also
  switch_selector_gpio[0].port = DIP_IN1_GPIO_Port;
  switch_selector_gpio[0].pin = DIP_IN1_Pin;
  switch_selector_gpio[1].port = DIP_IN2_GPIO_Port;
  switch_selector_gpio[1].pin = DIP_IN2_Pin;

  switch_selector_setup(&switch_selector, switch_selector_gpio, 2);

  //MOTION SENSOR SETUP
  //----------------------------------------------------------------------------

  switch(switch_selector.value)
  {
#ifdef DEBUG_MEMORY
	case MOTION_SWITCH_MODE_0:
		motion_initial_conf.threshold = OFFICE_MOTION_SENSOR_DETECTION_THRESHOLD;
		motion_initial_conf.window_time = OFFICE_MOTION_SENSOR_WINDOW;
	  break;
	case MOTION_SWITCH_MODE_1:
		motion_initial_conf.threshold = RESIDENTIAL_MOTION_SENSOR_DETECTION_THRESHOLD;
		motion_initial_conf.window_time = RESIDENTIAL_MOTION_SENSOR_WINDOW;
	  break;
	case MOTION_SWITCH_MODE_2:
		motion_initial_conf.threshold = RESIDENTIAL_MOTION_SENSOR_DETECTION_THRESHOLD;
		motion_initial_conf.window_time = RESIDENTIAL_MOTION_SENSOR_WINDOW;
		//do nothing
		__NOP();
		break;
#endif //DEBUG_MEMORY
	case MOTION_SWITCH_MODE_3:
		//do nothing
		motion_initial_conf.threshold = SURFACESHIELD_MOTION_SENSOR_DETECTION_THRESHOLD;
		motion_initial_conf.window_time =SURFACESHIELD_MOTION_SENSOR_WINDOW;
		break;
	default:
		//do nothing
		__NOP();
	  break;
  }

  //Good initial conf.
  motion_initial_conf.blind_time = PYD1598_BT_0_5_SEC;
  motion_initial_conf.count_mode = PYD1598_COUNT_MODE_NO_BPF_SIGN_CHANGE;
  motion_initial_conf.hpf_cutoff = PYD1598_HPF_0_2HZ;
  motion_initial_conf.op_mode = PYD1598_WAKE_UP;
  motion_initial_conf.pulse_counter = PYD1598_PC_2_PULSES;
  motion_initial_conf.signal_source = PYD1598_SOURCE_PIR_BFP;

  //Hardware assignation:
  pyd1598_hardware_interface_t setin_pin;
  pyd1598_hardware_interface_t direct_link_pin;

  setin_pin.pin = SERIN_Pin;
  setin_pin.port = SERIN_GPIO_Port;
  setin_pin.irq_type = EXTI4_15_IRQn;
  direct_link_pin.pin = DIRLINK_Pin;
  direct_link_pin.port = DIRLINK_GPIO_Port;
  direct_link_pin.irq_type = EXTI4_15_IRQn;

  pyd1598_setup(&motion_sensor, motion_initial_conf, setin_pin, direct_link_pin);

//  pyd1598_serin_set_zero_pulse(setin_pin);
  HAL_Delay(1000);

  pyd1598_request_write(&motion_sensor);

  //LIGHTS SETUP
  //----------------------------------------------------------------------------

  relay_t light_1;
  relay_t light_2;
  relay_t light_uv;

  deadline.msec = 80;
  deadline_timer_setup(&deadline_timer_light_1, deadline);
  deadline_timer_setup(&deadline_timer_light_2, deadline);
  deadline_timer_setup(&deadline_timer_uv, deadline);

  //Hardware assignation:

  relay_gpio_t light_1_output_a;
  relay_gpio_t light_1_output_b;
  relay_gpio_t light_2_output_a;
  relay_gpio_t light_2_output_b;
  relay_gpio_t light_uv_output_a;
  relay_gpio_t light_uv_output_b;

  light_1_output_a.pin = LAMP1_OUTA_Pin;
  light_2_output_a.pin = LAMP2_OUTA_Pin;
  light_uv_output_a.pin = UV_OUTA_Pin;
  light_1_output_b.pin = LAMP1_OUTB_Pin;
  light_2_output_b.pin = LAMP2_OUTB_Pin;
  light_uv_output_b.pin = UV_OUTB_Pin;

  light_1_output_a.port = LAMP1_OUTA_GPIO_Port;
  light_2_output_a.port = LAMP2_OUTA_GPIO_Port;
  light_uv_output_a.port = UV_OUTA_GPIO_Port;
  light_1_output_b.port = LAMP1_OUTB_GPIO_Port;
  light_2_output_b.port = LAMP2_OUTB_GPIO_Port;
  light_uv_output_b.port = UV_OUTB_GPIO_Port;

  relay_setup(&light_1, light_1_output_a, light_1_output_b);
  relay_setup(&light_2, light_2_output_a, light_2_output_b);
  relay_setup(&light_uv, light_uv_output_a, light_uv_output_b);


  //BUTTONS SETUP
  //----------------------------------------------------------------------------

  //timers config:
  timer_buttons.msec = 100;
  deadline_timer_setup(&deadline_buttons, timer_buttons);

  //Hardware assignation:
  button_gpio_t button_input;
  button_input.pin = LAMP1_IN_Pin;
  button_input.port = LAMP1_IN_GPIO_Port;
  button_setup(&button_light_1, button_input);

  button_input.pin = LAMP2_IN_Pin;
  button_input.port = LAMP2_IN_GPIO_Port;
  button_setup(&button_light_2, button_input);

  button_input.pin = UV_IN_Pin;
  button_input.port = UV_IN_GPIO_Port;
  button_setup(&button_uv, button_input);


  //SIGNAL LED SETUP
  //----------------------------------------------------------------------------
  led_signal_t signal_led;
  led_signal_gpio_t signal_led_gpio;
#ifdef DEBUG_MEMORY
  deadline_timer_expired_t indicator_timer_expired;
#endif //DEBUG_MEMORY
  signal_led_gpio.pin = LED_Pin;
  signal_led_gpio.port = LED_GPIO_Port;
  led_signal_setup(&signal_led, signal_led_gpio);
  deadline.msec = 500;
  deadline.sec = 0;

  deadline_timer_setup(&deadline_led_indicator, deadline);

  led_signal_type_selector(&signal_led, LED_SIGNAL_SOLID);

  //APPLICATION LIGHT SETUP
  //----------------------------------------------------------------------------
//TODO: (medium) create a typedef for all this


  /*
     * MOTION_SWITCH_MODE_0	=	OFFFICE
     * MOTION_SWITCH_MODE_1	=	RESIDENTIAL
     * MOTION_SWITCH_MODE_2	=
     * */
    //timers config:
    switch(switch_selector.value)
    {
#ifdef DEBUG_MEMORY
      case MOTION_SWITCH_MODE_0:
    	  timer_motion_light_1.msec = OFFICE_LAMP1_ON_TIME_MS;
    	  timer_motion_light_2.msec = OFFICE_LAMP2_ON_TIME_MS;
    	  timer_motion_uv_safe.msec = OFFICE_LAMP_UV_SAFETY_TIME_MS;
    	  timer_motion_uv.msec = OFFICE_LAMP_UV_ON_TIME_MS;
    	  timer_motion_uv_timeout.msec = OFFICE_LAMP_UV_TIMEOUT_MS;
    	  break;
      case MOTION_SWITCH_MODE_1:
    	  timer_motion_light_1.msec = RESIDENTIAL_LAMP1_ON_TIME_MS;
		  timer_motion_light_2.msec = RESIDENTIAL_LAMP2_ON_TIME_MS;
		  timer_motion_uv_safe.msec = RESIDENTIAL_LAMP_UV_SAFETY_TIME_MS;
		  timer_motion_uv.msec = RESIDENTIAL_LAMP_UV_ON_TIME_MS;
		  timer_motion_uv_timeout.msec = RESIDENTIAL_LAMP_UV_TIMEOUT_MS;
    	  break;
      case MOTION_SWITCH_MODE_2:
    	  timer_motion_light_1.msec = RESIDENTIAL_LAMP1_ON_TIME_MS;
		  timer_motion_light_2.msec = RESIDENTIAL_LAMP2_ON_TIME_MS;
		  timer_motion_uv_safe.msec = RESIDENTIAL_LAMP_UV_SAFETY_TIME_MS;
		  timer_motion_uv.msec = RESIDENTIAL_LAMP_UV_ON_TIME_MS;
		  timer_motion_uv_timeout.msec = RESIDENTIAL_LAMP_UV_TIMEOUT_MS;
    	  break;
#endif //DEBUG_MEMORY
      case MOTION_SWITCH_MODE_3:
    	  timer_motion_light_1.msec = SURFACESHIELD_LAMP1_ON_TIME_MS;
		  timer_motion_light_2.msec = SURFACESHIELD_LAMP2_ON_TIME_MS;
		  timer_motion_uv_safe.msec = SURFACESHIELD_LAMP_UV_SAFETY_TIME_MS;
		  timer_motion_uv.msec = SURFACESHIELD_LAMP_UV_ON_TIME_MS;
		  timer_motion_uv_timeout.msec = SURFACESHIELD_LAMP_UV_TIMEOUT_MS;
    	  __NOP();
    	  break;
      default:
    	  //do nothing
    	  __NOP();
    	  break;
    }

  //Light 1
  deadline_timer_setup(&deadline_motion_light_1, timer_motion_light_1);
//  deadline_timer_setup_shared_clock(&deadline_motion_light_1, &general_clock,
//		  	  	  	  	  	  	  	  	  	  timer_motion_light_1);
  light_1_state = MOTION_LIGHT_IDLE;
  //Light 2
  deadline_timer_setup(&deadline_motion_light_2, timer_motion_light_1);
  light_2_state = MOTION_LIGHT_IDLE;
  //Light UV
  deadline_timer_setup(&deadline_motion_uv, timer_motion_uv);
  //Light safe UV
  deadline_timer_setup(&deadline_motion_uv_safe, timer_motion_uv_safe);
  //Light UV Timeout
  deadline_timer_setup(&deadline_motion_uv_timeout, timer_motion_uv_timeout);
  //Light UV Wait Timeout
  deadline.msec = 5000;
  deadline_timer_setup(&deadline_uv_wait_timeout, deadline);





  //MODE 2 Setup (NuTone)
  //----------------------------------------------------------------------------
  nutone_t exhaust_fan;
  //  nutone_app_t nutone_app_hand;
  button_handler_t button_fan;
  button_handler_t button_white;
  button_handler_t button_vyv;
  motion_handler_t motion_sensor_hand;
  led_signal_handler_t led_signal_hand;
  nutone_relay_handler_t lights;
  nutone_relay_handler_t fan;


  if(switch_selector.value == MOTION_SWITCH_MODE_3)
  {


	  lights.relay = &light_uv;
	  lights.deadline_timer = &deadline_timer_light_1;
	  fan.relay = &light_2;//This is not a UV relay but a simple relay
	  fan.deadline_timer = &deadline_timer_light_2;

	  nutone_setup(&exhaust_fan, &lights, &fan, &ctrl_timer, &vyv_timeoff);

	  button_white.button = &button_light_1;
	  button_white.btn_timer = &deadline_buttons;
	  button_fan.button = &button_light_2;
	  button_fan.btn_timer = &deadline_buttons;
	  button_vyv.button = &button_uv;
	  button_vyv.btn_timer = &deadline_buttons;
	  motion_sensor_hand.motion_sensor = &motion_sensor;

	  led_signal_hand.led_signal = &signal_led;
	  led_signal_hand.led_signal_timer = &deadline_led_indicator;

	  nutone_app_config(&nutone_app_hand, &exhaust_fan, &motion_sensor_hand,
			  	  	  	  	  &button_fan, &button_white, &button_vyv,
							  &led_signal_hand,
							  &deadline_motion_light_2,
							  &deadline_motion_light_1,
							  &deadline_motion_uv,
							  &deadline_motion_uv_timeout,
							  &deadline_motion_uv_safe);
	  led_signal_start(&signal_led);
	  signal_led.type = LED_SIGNAL_BLINK;

	  uint8_t idx = 0;
	  relay_off(lights.relay);
	  HAL_Delay(100);
	  relay_deenergize(lights.relay);
	  for(idx = 0; idx < 10; idx++)
	  {
		  HAL_Delay(1300);
		  led_signal_fsm(led_signal_hand.led_signal);
	  }
	  led_signal_stop(led_signal_hand.led_signal);


  }


  //Start Timers
  //----------------------------------------------------------------------------
	//Start interrupts:
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim14);//SERIN communication timer
	HAL_TIM_Base_Start_IT(&htim16);//Used to generate a clock
	//Direct Link communication timer
	HAL_NVIC_SetPriority(TIM17_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM17_IRQn);
	HAL_TIM_Base_Start_IT(&htim17);
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	HAL_NVIC_SetPriority(EXTI2_3_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  //Set initial states:
  //----------------------------------------------------------------------------
  relay_ask_off_pulse_fsm(&light_1);
  relay_ask_off_pulse_fsm(&light_2);
  relay_ask_off_pulse_fsm(&light_uv);


  uv_state = MOTION_LIGHT_UV_IDLE;
  abort_uv = MOTION_ABORT_FALSE;
  //Light timeout UV

  //motion switch
  motion_sensed_light_1 = MOTION_ISR_ATTENDED;
  motion_sensed_light_2 = MOTION_ISR_ATTENDED;
  motion_sensed_uv = MOTION_ISR_ATTENDED;

  led_signal_stop(&signal_led);



#ifdef DEBUG_REYNOLDS
#endif //DEBUG_REYNOLDS


  /**************************************************************************/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/**************************************************************************/



	  switch(switch_selector.value)
	  {
		  case MOTION_SWITCH_MODE_0:
#ifdef DEBUG_MEMORY
			  events_detection_uv_waits(&motion_sensor, &deadline_buttons,
								  &button_light_1, &button_light_2, &button_uv,
								  &motion_sensed_light_1, &motion_sensed_light_2,
								  &motion_sensed_uv,
								  &abort_uv);
			  motion_light_control_fsm(&light_1, &button_light_1, &motion_sensor,
									&deadline_motion_light_1, &light_1_state,
									&motion_sensed_light_1);
			  motion_light_control_fsm(&light_2, &button_light_2, &motion_sensor,
									&deadline_motion_light_2, &light_2_state,
									&motion_sensed_light_2);

			  if((light_1.relay_status == RELAY_ON) ||
				 (light_2.relay_status == RELAY_ON))
			  {
				  wait = MOTION_UV_WAIT_TRUE;
			  }
			  else
			  {
				  wait = MOTION_UV_WAIT_FALSE;
			  }

			  motion_uv_ctrl_wait_fsm(&light_uv, &button_uv, &motion_sensor,
								&deadline_motion_uv, &deadline_motion_uv_safe,
								&deadline_motion_uv_timeout,
								&deadline_uv_wait_timeout,
								&uv_state, &motion_sensed_uv, &abort_uv, wait,
								&signal_led);



			  //Actuator routines:
			  output_fsm_ctrl(&light_1, &deadline_timer_light_1);
			  output_fsm_ctrl(&light_2, &deadline_timer_light_2);
			  output_fsm_ctrl(&light_uv, &deadline_timer_uv);

			  //LED indicator
			  deadline_timer_check(&deadline_led_indicator, &indicator_timer_expired);

			  if(indicator_timer_expired == TIMER_EXPIRED_TRUE)
			  {
				  led_signal_fsm(&signal_led);
				  deadline_timer_set_initial_time(&deadline_led_indicator);
			  }
#endif //DEBUG_MEMORY

			  break;

		  case MOTION_SWITCH_MODE_1:
#ifdef DEBUG_MEMORY
			  events_detection_uv_waits(&motion_sensor, &deadline_buttons,
			  								  &button_light_1, &button_light_2, &button_uv,
			  								  &motion_sensed_light_1, &motion_sensed_light_2,
			  								  &motion_sensed_uv,
			  								  &abort_uv);

			  motion_light_control_fsm(&light_1, &button_light_1, &motion_sensor,
									&deadline_motion_light_1, &light_1_state,
									&motion_sensed_light_1);

			  direct_light_control_fsm(&light_2, &button_light_2,
												  &deadline_motion_light_2,
												  &light_2_no_motion_state);

			  if(light_1.relay_status == RELAY_ON)
			  {
				  wait = MOTION_UV_WAIT_TRUE;
			  }
			  else
			  {
				  wait = MOTION_UV_WAIT_FALSE;
			  }

			  motion_uv_ctrl_wait_fsm(&light_uv, &button_uv, &motion_sensor,
								&deadline_motion_uv, &deadline_motion_uv_safe,
								&deadline_motion_uv_timeout,
								&deadline_uv_wait_timeout,
								&uv_state, &motion_sensed_uv, &abort_uv, wait,
								&signal_led);


			  //Actuator routines:
			  output_fsm_ctrl(&light_1, &deadline_timer_light_1);
			  output_fsm_ctrl(&light_2, &deadline_timer_light_2);
			  output_fsm_ctrl(&light_uv, &deadline_timer_uv);

			  //LED indicator
			  deadline_timer_check(&deadline_led_indicator, &indicator_timer_expired);

			  if(indicator_timer_expired == TIMER_EXPIRED_TRUE)
			  {
				  led_signal_fsm(&signal_led);
				  deadline_timer_set_initial_time(&deadline_led_indicator);
			  }
#endif

			  break;
		  case MOTION_SWITCH_MODE_2:
			  //do nothing
			  signal_led.type = LED_SIGNAL_BLINK;

			  break;
		  case MOTION_SWITCH_MODE_3:
			  nutone_app_process(&nutone_app_hand);

			  break;
		  default:
			  //do nothing
			  __NOP();
			  break;
	  }


#ifdef DEBUG_REYNOLDS
//	  printf("Hello World %d \n\r", test);
//	  HAL_Delay(1000);
#endif //DEBUG_REYNOLDS


  }
  /**************************************************************************/
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/***************************Application Functions******************************/

/*----------------------Motion sensor Application routines--------------------*/

//TODO: (medium) all the digital outputs behave light a light. Last requirement
//changes added other kind of actuators. Change the HAL layer and middleware
//layer to set a proper name


//#define TESTING_SENSOR

void events_detection_uv_waits(pyd1598_sensor_t *motion,
							deadline_timer_t *deadline_events,
							button_t *button_lamp_1, button_t *button_lamp_2,
							button_t *button_lamp_uv,
							motion_sensed_t *motion_light_1,
							motion_sensed_t *motion_light_2,
							motion_sensed_t *motion_uv,
							motion_light_uv_abort_t *abort_signal_uv)
{
	//variables to check motion events
	pyd1598_motion_isr_status_t motion_isr_status = PYD1598_MOTION_ISR_ATTENDED;

	//To check button states:
	button_isr_status_t button_isr_stat;
	button_edge_t check_edge;


	//Reading if there is a motion interrupt pending to solve
#ifndef TESTING_SENSOR
	pyd1598_read_wakeup_signal(motion, &motion_isr_status);
#endif //TESTING_SENSOR
	//Activating flags
	if(motion_isr_status == PYD1598_MOTION_ISR_UNATTENDED)
	{
		*motion_light_1 = MOTION_ISR_UNATTENDED;
		*motion_light_2 = MOTION_ISR_UNATTENDED;
		*motion_uv = MOTION_ISR_UNATTENDED;
		motion->motion_sensed = PYD1598_MOTION_ISR_ATTENDED;
	}

	//Reading Buttons
	sense_button_event(deadline_events, button_lamp_1);
	sense_button_event(deadline_events, button_lamp_2);
	sense_button_event(deadline_events, button_lamp_uv);

#ifdef ABORT_WITH_BUTTONS
	//Activating a switch while uv fsm is running turn off UV lamp
	button_check_isr_request(*button_lamp_1, &button_isr_stat, &check_edge);
	if(button_isr_stat == BUTTON_ISR_UNATTENDED)
	{
		*abort_signal_uv = MOTION_ABORT_TRUE;
	}

	button_check_isr_request(*button_lamp_2, &button_isr_stat, &check_edge);
	if(button_isr_stat == BUTTON_ISR_UNATTENDED)
	{
		*abort_signal_uv = MOTION_ABORT_TRUE;
	}
#endif //ABORT_WITH_BUTTONS

	//Since a push button does not maintain its state, memory is needed.
	button_check_isr_request(*button_lamp_uv, &button_isr_stat, &check_edge);
	if(button_isr_stat == BUTTON_ISR_UNATTENDED)
	{
		if(button_lamp_uv->push_status != BUTTON_PUSH_ON)
		{
			button_lamp_uv->push_status = BUTTON_PUSH_ON;
		}
		else
		{
			button_lamp_uv->push_status = BUTTON_PUSH_OFF;
		}
	}
}

void sense_button_event(deadline_timer_t *deadline_events, button_t *button)
{

	deadline_timer_expired_t timer_expired;
	if(button->debounce_lock == BUTTON_DEBOUNCE_LOCK_ON)
	{
		//TODO: (medium) Check if a new timer should be configured
		deadline_timer_check(deadline_events, &timer_expired);

		if(timer_expired == TIMER_EXPIRED_TRUE)
		{
			button_debounce_fsm(button);
			deadline_timer_set_initial_time(deadline_events);
		}
	}


}

void motion_light_control_fsm(relay_t *light,
								button_t *button,
								pyd1598_sensor_t *motion_sensor,
								deadline_timer_t *deadline_timer,
								motion_light_state_t *fsm_state,
								motion_sensed_t *motion_sensed)
{

	button_isr_status_t button_isr_status;
	button_edge_t edge = button->edge;

	button_check_isr_request(*button, &button_isr_status, &edge);

	if( (button_isr_status == BUTTON_ISR_UNATTENDED) ||
		(*motion_sensed == MOTION_ISR_UNATTENDED))
	{
		if(button_isr_status == BUTTON_ISR_UNATTENDED)
		{
			button_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;

			if(edge == BUTTON_EDGE_NEGATIVE)
			{
				*fsm_state = MOTION_LIGHT_TURN_OFF_LIGHT;
			}
			else
			{
				*fsm_state = MOTION_LIGHT_CHECK_BUTTON;
			}
		}

		if( (*motion_sensed == MOTION_ISR_UNATTENDED) &&
			(*fsm_state == MOTION_LIGHT_IDLE))
		{
			*fsm_state = MOTION_LIGHT_CHECK_BUTTON;
			*motion_sensed = MOTION_ISR_ATTENDED;
		}

	}

	switch(*fsm_state)
	{
		case MOTION_LIGHT_IDLE:

			__NOP();//Do nothing

			break;
		case MOTION_LIGHT_CHECK_BUTTON:

			button_status_t button_status = BUTTON_OFF;
			button_get_status(button, &button_status);

			if(button_status == BUTTON_ON)
			{
				*fsm_state = MOTION_LIGHT_TURN_ON_LIGHT;
				*motion_sensed = MOTION_ISR_ATTENDED;
			}
			else
			{
				 //if an edge where detected an error occurred in the button
				*fsm_state = MOTION_LIGHT_TURN_OFF_LIGHT;
				*motion_sensed = MOTION_ISR_ATTENDED;
			}

			break;
		case MOTION_LIGHT_TURN_ON_LIGHT:

			//this is done in another fsm
			relay_ask_on_pulse_fsm(light);
			*fsm_state = MOTION_LIGHT_INIT_TIMER;

			break;
		case MOTION_LIGHT_INIT_TIMER:
			//This updates the timer count
			deadline_timer_set_initial_time(deadline_timer);
			*fsm_state = MOTION_LIGHT_WAIT_EXPIRATION;

			break;
		case MOTION_LIGHT_WAIT_EXPIRATION:

			deadline_timer_expired_t deadline_expired;

			deadline_timer_check(deadline_timer, &deadline_expired);

			if(deadline_expired == TIMER_EXPIRED_TRUE)
			{
				*fsm_state = MOTION_LIGHT_TURN_OFF_LIGHT;
			}
			else
			{
			}

			if(*motion_sensed == MOTION_ISR_UNATTENDED)
			{
				*motion_sensed = MOTION_ISR_ATTENDED;
				*fsm_state = MOTION_LIGHT_CHECK_BUTTON;
			}
			break;
		case MOTION_LIGHT_TURN_OFF_LIGHT:

			deadline_timer_force_expiration(deadline_timer);
			//this is done in another fsm
			relay_ask_off_pulse_fsm(light);
			*fsm_state = MOTION_LIGHT_IDLE;
			break;
		default:
			*fsm_state = MOTION_LIGHT_IDLE;
			break;
	}

	button->edge_attended = button_isr_status;

}

void motion_uv_ctrl_wait_fsm(relay_t *light_uv,
									button_t *button_uv,
									pyd1598_sensor_t *motion_sensor,
									deadline_timer_t *deadline_timer,
									deadline_timer_t *deadline_safe_timer,
									deadline_timer_t *deadline_timeout,
									deadline_timer_t *deadline_wait_timeout,
									motion_light_uv_state_t *fsm_state,
									motion_sensed_t *motion_sensed,
									motion_light_uv_abort_t *uv_abort,
									motion_uv_wait_signal_t wait,
									led_signal_t *signal)
{
	button_isr_status_t button_isr_status;
	button_edge_t edge = button_uv->edge;
	pyd1598_motion_isr_status_t motion_isr_status;
	deadline_timer_expired_t deadline_safe_expired;
	deadline_timer_expired_t deadline_timeout_expired;

	button_check_isr_request(*button_uv, &button_isr_status, &edge);
	pyd1598_check_isr_request(*motion_sensor, &motion_isr_status);


	if(button_isr_status == BUTTON_ISR_UNATTENDED)
	{

		//Change if abort is not required with a second push
		if(button_uv->push_status == BUTTON_PUSH_ON)
		{
			*fsm_state = MOTION_LIGHT_UV_INIT_TIMEOUT_TIMER;
		}
		else
		{
			*fsm_state = MOTION_LIGHT_UV_ABORT;
		}

		button_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;
	}

	if(*uv_abort == MOTION_ABORT_TRUE)
	{
		*fsm_state = MOTION_LIGHT_UV_ABORT;
		*uv_abort = MOTION_ABORT_FALSE;
	}


	switch(*fsm_state)
	{
		case MOTION_LIGHT_UV_IDLE:

			__NOP();//Do nothing

			break;
		case MOTION_LIGHT_UV_INIT_TIMEOUT_TIMER:
			//Initialize deadline_timeout
			deadline_timer_set_initial_time(deadline_timeout);
			deadline_timer_set_initial_time(deadline_wait_timeout);

			//Start LED indicator
			led_signal_start(signal);
			signal->type = LED_SIGNAL_BLINK;

			*fsm_state = MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT;

			break;
#ifndef TEST_TIMEOUT
		case MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT:

			if(*motion_sensed == MOTION_ISR_UNATTENDED)
			{
				*motion_sensed = MOTION_ISR_ATTENDED;
			}


			deadline_timer_check(deadline_timeout, &deadline_timeout_expired);

			if(deadline_timeout_expired == TIMER_EXPIRED_TRUE)//This should be a long timer
			{
				*motion_sensed = MOTION_ISR_ATTENDED;
				*fsm_state = MOTION_LIGHT_UV_INIT_SAFE_TIMER;
				signal->type = LED_SIGNAL_SOLID;
			}



			break;
#endif //TEST_TIMEOUT

#ifdef TEST_TIMEOUT
		case MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT:

			deadline_timer_check(deadline_wait_timeout,
									&deadline_wait_timeout_expired);

			if(deadline_wait_timeout_expired == TIMER_EXPIRED_TRUE)
			{
				*fsm_state = MOTION_LIGHT_UV_CHECK_TIMEOUT_TIMER;
			}
			__NOP();//Do nothing

			break;
		case MOTION_LIGHT_UV_CHECK_TIMEOUT_TIMER:

			deadline_timer_check(deadline_timeout, &deadline_timeout_expired);

			if(deadline_timeout_expired == TIMER_EXPIRED_TRUE)//This should be a long timer
			{
				*fsm_state = MOTION_LIGHT_UV_ABORT;
			}
			else
			{
				if(*motion_sensed == MOTION_ISR_UNATTENDED)
				{
					deadline_timer_set_initial_time(deadline_wait_timeout);
					*motion_sensed = MOTION_ISR_ATTENDED;
					*fsm_state = MOTION_LIGHT_UV_WAIT_MOTION_TIMEOUT;
				}
				else
				{
					*fsm_state = MOTION_LIGHT_UV_INIT_SAFE_TIMER;
				}
			}

			break;
#endif //TEST_TIMEOUT

		case MOTION_LIGHT_UV_INIT_SAFE_TIMER:
			//TODO: (high) add a timeout
			deadline_timer_set_initial_time(deadline_safe_timer);

			*fsm_state = MOTION_LIGHT_UV_WAIT_SAFE_TIMER;
			break;

		case MOTION_LIGHT_UV_WAIT_SAFE_TIMER:

			deadline_timer_check(deadline_safe_timer, &deadline_safe_expired);

			if(deadline_safe_expired == TIMER_EXPIRED_TRUE)//This should be a long timer
			{
				*fsm_state = MOTION_LIGHT_UV_TURN_ON_LIGHT;
			}
			else
			{
				if(*motion_sensed == MOTION_ISR_UNATTENDED)
				{
					*motion_sensed = MOTION_ISR_ATTENDED;
					*fsm_state = MOTION_LIGHT_UV_ABORT;
				}
				else
				{
					if(wait == MOTION_UV_WAIT_TRUE)
					{
						*fsm_state = MOTION_LIGHT_UV_INIT_SAFE_TIMER;
					}
				}

			}

			break;
		case MOTION_LIGHT_UV_TURN_ON_LIGHT:
			//this is done in another fsm
			relay_ask_on_pulse_fsm(light_uv);
//			signal->type = LED_SIGNAL_SOLID;
			*fsm_state = MOTION_LIGHT_UV_INIT_TIMER;

			break;
		case MOTION_LIGHT_UV_INIT_TIMER:
			deadline_timer_set_initial_time(deadline_timer);
			*fsm_state = MOTION_LIGHT_UV_WAIT_EXPIRATION;
			break;
		case MOTION_LIGHT_UV_WAIT_EXPIRATION:
			deadline_timer_expired_t deadline_expired;
			deadline_timer_check(deadline_timer, &deadline_expired);

			if(deadline_expired == TIMER_EXPIRED_TRUE)
			{
				*fsm_state = MOTION_LIGHT_UV_TURN_OFF_LIGHT;
			}
			else
			{
				//Do nothing
			}

			if(*motion_sensed == MOTION_ISR_UNATTENDED)
			{
				*motion_sensed = MOTION_ISR_ATTENDED;
				*fsm_state = MOTION_LIGHT_UV_ABORT;
			}

			break;
		case MOTION_LIGHT_UV_ABORT:

			*fsm_state = MOTION_LIGHT_UV_TURN_OFF_LIGHT;
			break;
		case MOTION_LIGHT_UV_TURN_OFF_LIGHT:
			//this is done in another fsm
			deadline_timer_force_expiration(deadline_timer);
			deadline_timer_force_expiration(deadline_timeout);
			deadline_timer_force_expiration(deadline_wait_timeout);
			button_uv->push_status = BUTTON_PUSH_OFF;


			//Send signal to turn off UV lamp
			relay_ask_off_pulse_fsm(light_uv);

			//Send signal stop LED indicator
			led_signal_stop(signal);


			*fsm_state = MOTION_LIGHT_UV_IDLE;
			break;
		default:
			*fsm_state = MOTION_LIGHT_UV_IDLE;
			break;
	}

	button_uv->edge_attended = button_isr_status;
}


void direct_light_control_fsm(relay_t *light,
								button_t *button,
								deadline_timer_t *deadline_timer,
								motion_light_no_motion_state_t *fsm_state)
{

	button_isr_status_t button_isr_status;
	button_edge_t edge = button->edge;
	button_status_t button_status = BUTTON_OFF;

	button_check_isr_request(*button, &button_isr_status, &edge);

	if( button_isr_status == BUTTON_ISR_UNATTENDED)
	{
		button_isr_status = PYD1598_WAKEUP_ISR_ATTENDED;

		if(edge == BUTTON_EDGE_NEGATIVE)
		{
			*fsm_state = NO_MOTION_LIGHT_TURN_OFF_LIGHT;
		}
		else
		{
			*fsm_state =  NO_MOTION_LIGHT_TURN_ON_LIGHT;
		}
	}

	switch(*fsm_state)
	{
		case NO_MOTION_LIGHT_IDLE:

			__NOP();//Do nothing

			break;
		case NO_MOTION_LIGHT_TURN_ON_LIGHT:

			//this is done in another fsm

			relay_ask_on_pulse_fsm(light);
			*fsm_state = NO_MOTION_LIGHT_CHECK_BUTTON;

			break;
		case NO_MOTION_LIGHT_CHECK_BUTTON:
			//This updates the timer count
			button_get_status(button, &button_status);

			//TODO: (low) This step is just to ensure things are done
			if(button_status == BUTTON_OFF)
			{
				*fsm_state = NO_MOTION_LIGHT_TURN_OFF_LIGHT;
			}

			break;
		case NO_MOTION_LIGHT_TURN_OFF_LIGHT:
			//this is done in another fsm
			relay_ask_off_pulse_fsm(light);
			*fsm_state = NO_MOTION_LIGHT_IDLE;
			break;
		default:
			*fsm_state = NO_MOTION_LIGHT_IDLE;
			break;
	}

	button->edge_attended = button_isr_status;

}




/**********************************ISR*****************************************/

// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim == &htim17)
	{
		pyd1598_dl_readout_fsm(&motion_sensor.direct_link,
											motion_sensor.serin.config.op_mode);
	}
	if (htim == &htim14)
	{
		pyd1598_serin_send_datagram_fsm(&motion_sensor.serin,
								motion_sensor.direct_link.hardware_inteface);

	}

	if (htim == &htim1)
	{
//		increment_timer();
	}

	if (htim == &htim16)
	{

		//TODO: (low)can I change this to the infinite while loop in the main?
		deadline_timer_count(&deadline_timer_light_1);
		deadline_timer_count(&deadline_timer_light_2);
		deadline_timer_count(&deadline_timer_uv);
		deadline_timer_count(&deadline_buttons);
		//
		deadline_timer_count(&deadline_motion_light_1);
		deadline_timer_count(&deadline_motion_light_2);
		deadline_timer_count(&deadline_motion_uv);
		deadline_timer_count(&deadline_motion_uv_safe);

		deadline_timer_count(&deadline_led_indicator);

		deadline_timer_count(&deadline_motion_uv_timeout);
		deadline_timer_count(&deadline_uv_wait_timeout);

		deadline_timer_count(&ctrl_timer);
		deadline_timer_count(&vyv_timeoff);

		deadline_timer_increment(&general_clock);

	}



}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{


	if(GPIO_Pin == motion_sensor.direct_link.hardware_inteface.pin)
	{
//		pyd1598_direct_link_isr_init_set();
		motion_sensor.direct_link.start_fsm = true;
		TIM17->CNT = 0;
	}
	else
	{
		__NOP();
	}

	if(GPIO_Pin == button_light_1.hardware_input.pin)
	{
		if(button_light_1.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_positive_edge_detected(&button_light_1);
			deadline_timer_set_initial_time(&deadline_buttons);//TODO: (medium) Check if a new timer should be configured
		}

	}
	if(GPIO_Pin == button_light_2.hardware_input.pin)
	{
		if(button_light_2.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_positive_edge_detected(&button_light_2);
			deadline_timer_set_initial_time(&deadline_buttons);//TODO: (medium) Check if a new timer should be configured
		}
	}
	if(GPIO_Pin == button_uv.hardware_input.pin)
	{
		if(button_uv.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_positive_edge_detected(&button_uv);
			deadline_timer_set_initial_time(&deadline_buttons);//TODO: (medium) Check if a new timer should be configured
		}
	}
}


void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{


	if(GPIO_Pin == button_light_1.hardware_input.pin)
	{
		if(button_light_1.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_negative_edge_detected(&button_light_1);
			deadline_timer_set_initial_time(&deadline_buttons);
		}
	}
	if(GPIO_Pin == button_light_2.hardware_input.pin)
	{
		if(button_light_2.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_negative_edge_detected(&button_light_2);
			deadline_timer_set_initial_time(&deadline_buttons);//TODO: (medium) Check if a new timer should be configured
		}
	}

	if(GPIO_Pin == button_uv.hardware_input.pin)
	{
		if(button_uv.debounce_lock == BUTTON_DEBOUNCE_LOCK_OFF)
		{
			button_negative_edge_detected(&button_uv);
			deadline_timer_set_initial_time(&deadline_buttons);//TODO: (medium) Check if a new timer should be configured
		}
	}
}





/****************************Timing routines***********************************/




#ifdef DEBUG_UART_REYNOLDS
/**
  * @brief  Retargets the C library printf function to the USART.
  * @PAram  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

#endif //DEBUG_UART_REYNOLDS
/*******************************************************************************
 *******************************************************************************
 *******************************************************************************
 ******************************************************************************/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
