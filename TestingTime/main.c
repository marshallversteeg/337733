
#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_delay.c"
#include "nrf_gpio.h"
//#include "nordic_common.h"
#include "app_gpiote.h"
#include "app_gpiote.c"
#include "app_timer.h"
#include "softdevice_handler.h"

//pin 2: Trigger Out
//pin 3: Echo In
//pin 4: Trigger Button In
//pin 5: Echo Response Out

//****************** Pinout: ******************//
#define US_Trig_1	5	// Center HC-SR04 Trigger 		//ADC6
#define US_Echo_1	6	// Center HC-SR04 Echo 			//ADC7
#define US_Trig_2	3	// Right HC-SR04 Trigger 		//ADC4
#define US_Echo_2	4	// Right HC-SR04 Echo 			//ADC5
#define US_Trig_3	1	// Left HC-SR04 Trigger 		//
#define US_Echo_3	20	// Left HC-SR04 Echo 			//
#define US_Line		2	// Line driver to power USs.	//ADC3
#define PIR_1		23	//Front 						//P23
#define PIR_2		24	//Back 							//P24

#define APP_GPIOTE_MAX_USERS    7
#define LINE_STARTUP			100

//*************** Global Vars: ***************//

// counter
static volatile uint32_t tCount = 0;
// count to us (micro seconds) conversion factor
// set in start_timer()
static volatile float countToUs = 1;
// exitDirection and entryDirection
static volatile uint8_t exitDir = -1;
static volatile uint8_t entryDir = -1;
// distance var
static volatile float distance = 500;
// Create a userID, which allocates space.
app_gpiote_user_id_t m_app_gpiote_my_id;

//********** Function Declarations: **********//
float personInRange();
void gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low);
bool getDistance(float* dist, uint8_t pinTrigger, uint8_t pinEcho);
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
void start_timer(void);
//********************************************//

void resetVars(){
	distance = 500;
	if(entryDir == 1 && exitDir == 0){
		nrf_gpio_pin_clear(LED_0);
		nrf_gpio_pin_clear(LED_1);
	} else if(entryDir == 0 && exitDir == 1){
		nrf_gpio_pin_clear(LED_0);
		nrf_gpio_pin_clear(LED_1);
	} else if(entryDir == 1 && exitDir == 1){
		nrf_gpio_pin_clear(LED_0);
		nrf_gpio_pin_set(LED_1);
	}  else if(entryDir == 0 && exitDir == 0){
		nrf_gpio_pin_set(LED_0);
		nrf_gpio_pin_clear(LED_1);
	}
	nrf_gpio_pin_set(LED_2);
	if(exitDir == 0)
	entryDir = -1;
	exitDir =-1;
}

void gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	if (event_pins_high_to_low & (1 << PIR_1)){
		// Check if PIR_2 is low
		// if(PIR_2 is low) stop US-Sampling
		// 		compare to initial direction
		// nrf_gpio_pin_set(LED_2);
		uint32_t PIR2val = nrf_gpio_pin_read(PIR_2);
		if(!PIR2val){
			exitDir = 1;
			resetVars();
		}
		// else do nothing
	}
	
	if (event_pins_low_to_high & (1 << PIR_1)){
		// Check if PIR_2 is low
		// if(PIR_2 is low) start US-Sampling
		// nrf_gpio_pin_clear(LED_2);
		uint32_t PIR2val = nrf_gpio_pin_read(PIR_2);
		if(!PIR2val){
			entryDir = 1;
			// nrf_gpio_pin_clear(US_Line);
			// nrf_delay_ms(LINE_STARTUP);
			distance = personInRange();
		}
		// 		store current direction
		// else do nothing
	}

	if (event_pins_high_to_low & (1 << PIR_2)){
		// Check if PIR_1 is low
		// if(PIR_1 is low) stop US-Sampling
		//		compare to initial direction
		uint32_t PIR1val = nrf_gpio_pin_read(PIR_1);
		if(!PIR1val){
			exitDir = 0;
			resetVars();
		}
		// else do nothing
	}

	if (event_pins_low_to_high & (1 << PIR_2)){
		// Check if PIR_1 is low
		// if(PIR_1 is low) start US-Sampling
		uint32_t PIR1val = nrf_gpio_pin_read(PIR_1);
		if(!PIR1val){
			entryDir = 0;
			// nrf_gpio_pin_clear(US_Line);
			// nrf_delay_ms(LINE_STARTUP);
			distance = personInRange();
		}
		// 		store current direction
		// else do nothing
	}
}

void app_error_handler(uint32_t error_code,
					   uint32_t line_num,
					   const uint8_t * p_file_name) {
	NVIC_SystemReset();
}

void configureGPIO(){
	uint32_t err_code;
	nrf_gpio_cfg_output(LED_0);
	nrf_gpio_cfg_output(LED_1);
	nrf_gpio_cfg_output(LED_2);
	nrf_gpio_cfg_output(US_Line);
	nrf_gpio_cfg_output(US_Trig_1);
	// nrf_gpio_cfg_output(US_Trig_2);
	// nrf_gpio_cfg_output(US_Trig_3);
	nrf_gpio_cfg_input(US_Echo_1, NRF_GPIO_PIN_PULLDOWN);
	// nrf_gpio_cfg_input(US_Echo_2, NRF_GPIO_PIN_PULLDOWN);
	// nrf_gpio_cfg_input(US_Echo_3, NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(PIR_1, NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(PIR_2, NRF_GPIO_PIN_PULLDOWN);
	// Create a bit mask for all of the events
	// allowing them to be triggered by the same
	// handler.
	nrf_gpio_pin_clear(US_Line);
	nrf_delay_ms(LINE_STARTUP);
	uint32_t gpiote_event_bit_mask = 0;
	// gpiote_event_bit_mask += (1 << US_Trig_1);
	// gpiote_event_bit_mask += (1 << US_Trig_2);
	// gpiote_event_bit_mask += (1 << US_Trig_3);
	// gpiote_event_bit_mask += (1 << US_Echo_1);
	// gpiote_event_bit_mask += (1 << US_Echo_2);
	// gpiote_event_bit_mask += (1 << US_Echo_3);
	gpiote_event_bit_mask += (1 << PIR_1);
	gpiote_event_bit_mask += (1 << PIR_2);
	// Init the users and events;
	APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
	err_code = app_gpiote_user_register(
		&m_app_gpiote_my_id,
		gpiote_event_bit_mask,
		gpiote_event_bit_mask,
		gpiote_event_handler
	);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_gpiote_user_enable(m_app_gpiote_my_id);
	APP_ERROR_CHECK(err_code);
}

// if person is in range determined by PIR
// begin to capture distance.
// After each distance capture set check
// to see if it is still in range.
float personInRange(){
	app_gpiote_user_disable(m_app_gpiote_my_id);
	uint32_t PIR_1read = nrf_gpio_pin_read(PIR_1);
	uint32_t PIR_2read = nrf_gpio_pin_read(PIR_2);
	// Init distance as a float
	float dist = 400.0;
	while(PIR_1read && PIR_2read){
		getDistance(&dist, US_Trig_1, US_Echo_1);
		PIR_1read = nrf_gpio_pin_read(PIR_1);
		PIR_2read = nrf_gpio_pin_read(PIR_2);
		// if(!(PIR_1read && PIR_2read)) break;
		// getDistance(&dist, US_Trig_2, US_Echo_2);
		// PIR_1read = nrf_gpio_pin_read(PIR_1);
		// PIR_2read = nrf_gpio_pin_read(PIR_2);
		// if(!(PIR_1read && PIR_2read)) break;
		// getDistance(&dist, US_Trig_3, US_Echo_3);
		// PIR_1read = nrf_gpio_pin_read(PIR_1);
		// PIR_2read = nrf_gpio_pin_read(PIR_2);
	}
	app_gpiote_user_enable(m_app_gpiote_my_id);
	return dist;
}

// get distance measurement from HC-SR04:
// Send a 10us HIGH pulse on the Trigger pin.
// The sensor sends out a “sonic burst” of 8 cycles.
// Listen to the Echo pin, and the duration of the next HIGH 
// signal will give you the time taken by the sound to go back 
// and forth from sensor to target.
// returns true only if a valid distance is obtained
bool getDistance(float* dist, uint8_t pinTrigger, uint8_t pinEcho)	
{
  // send 12us trigger pulse
  //    _
  // __| |__
  nrf_gpio_pin_clear(pinTrigger);
  nrf_delay_us(20);
  nrf_gpio_pin_set(pinTrigger);
  nrf_gpio_pin_clear(LED_2);
  nrf_delay_us(12);
  nrf_gpio_pin_clear(pinTrigger);
  nrf_delay_us(20);

  // listen for echo and time it
  //       ____________
  // _____|            |___
  
  // wait till Echo pin goes high
  while(!nrf_gpio_pin_read(pinEcho));
  // reset counter
  tCount = 0;
  // wait till Echo pin goes low
  while(nrf_gpio_pin_read(pinEcho) || (countToUs > 200));
  
  // calculate duration in us
  float duration = countToUs*tCount;
 
  // dist = duration * speed of sound * 1/2
  // dist in cm = duration in us * 10^-6 * 340.29 * 100 * 1/2
  float distance = duration*0.017;
  
  // check value
  if(distance < 400.0) {

	// save
	if(*dist > distance)
		*dist = distance;

	return true;
  }
  else {
	return false;
  }
}
// set up and start Timer1
void start_timer(void)
{   
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;  
  NRF_TIMER1->TASKS_CLEAR = 1;
  // set prescalar n
  // f = 16 MHz / 2^(n)
  uint8_t prescaler = 0;
	NRF_TIMER1->PRESCALER = prescaler; 
	NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_16Bit;

  // 16 MHz clock generates timer tick every 1/(16000000) s = 62.5 nano s
  // With compare enabled, the interrupt is fired every: 62.5 * comp1 nano s
  // = 0.0625*comp1 micro seconds
  // multiply this by 2^(prescalar)

  uint16_t comp1 = 500;
  // set compare
	NRF_TIMER1->CC[1] = comp1;

  // set conversion factor
  countToUs = 0.0625*comp1*(1 << prescaler);

  //Print the countToUS
  //printf("timer tick = %f us\n", countToUs);

  // enable compare 1
	NRF_TIMER1->INTENSET = 
	(TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);

  // use the shorts register to clear compare 1
  NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << 
						TIMER_SHORTS_COMPARE1_CLEAR_Pos);

  // enable IRQ
  NVIC_EnableIRQ(TIMER1_IRQn);
		
  // start timer
  NRF_TIMER1->TASKS_START = 1;
}

// Timer 1 IRQ handler
// just increment count
void TIMER1_IRQHandler(void)
{
	if (NRF_TIMER1->EVENTS_COMPARE[1] && 
	  NRF_TIMER1->INTENSET & TIMER_INTENSET_COMPARE1_Msk) {

	// clear compare register event 
	NRF_TIMER1->EVENTS_COMPARE[1] = 0;

	tCount++;
  }
}

int main(void){

	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);
	start_timer();
	configureGPIO();
	nrf_delay_ms(1000);
//	uint8_t inputRead;
//	app_timer_cnt_get(&x);
	while(1){
		// inputRead = nrf_gpio_port_read(3);
		// if(inputRead) nrf_gpio_pin_set(2);
		// else nrf_gpio_pin_clear(2);
		// nrf_delay_us(1);
		sd_app_evt_wait();
	}
}


