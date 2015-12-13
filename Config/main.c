
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
#define US_Trig_2	1	// Right HC-SR04 Trigger 		//
#define US_Echo_2	7	// Right HC-SR04 Echo 			//
#define US_Trig_3	3	// Left HC-SR04 Trigger 		//
#define US_Echo_3	4	// Left HC-SR04 Echo 			//
#define US_Line		2	// Line driver to power USs.	//ADC3
#define PIR_1		23	//Front 						//P23 B4
#define PIR_2		24	//Back 							//P24 B3

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

void gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
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
	//nrf_gpio_cfg_output(LED_2);
	nrf_gpio_cfg_output(US_Line);
	nrf_gpio_cfg_output(US_Trig_1);
	nrf_gpio_cfg_output(US_Trig_2);
	nrf_gpio_cfg_output(US_Trig_3);
	nrf_gpio_cfg_input(US_Echo_1, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(US_Echo_2, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(US_Echo_3, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIR_1, NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(PIR_2, NRF_GPIO_PIN_PULLDOWN);
	// Create a bit mask for all of the events
	// allowing them to be triggered by the same
	// handler.
	uint32_t gpiote_event_bit_mask = 0;
	// gpiote_event_bit_mask += (1 << US_Trig_1);
	// gpiote_event_bit_mask += (1 << US_Trig_2);
	// gpiote_event_bit_mask += (1 << US_Trig_3);
	// gpiote_event_bit_mask += (1 << US_Echo_1);
	// gpiote_event_bit_mask += (1 << US_Echo_2);
	// gpiote_event_bit_mask += (1 << US_Echo_3);
	// gpiote_event_bit_mask += (1 << PIR_1);
	// gpiote_event_bit_mask += (1 << PIR_2);
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


int main(void){

	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);
	configureGPIO();
	
//	uint8_t inputRead;
//	app_timer_cnt_get(&x);
	while(1){
		uint32_t PIR1 = nrf_gpio_pin_read(PIR_1);
		uint32_t PIR2 = nrf_gpio_pin_read(PIR_2);
		if(PIR1) nrf_gpio_pin_clear(LED_0);
		else nrf_gpio_pin_set(LED_0);
		if(PIR2) nrf_gpio_pin_clear(LED_1);
		else nrf_gpio_pin_set(LED_1);
		nrf_delay_ms(10);
	}
}


