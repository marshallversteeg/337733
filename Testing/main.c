
#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
//#include "nordic_common.h"
#include "app_gpiote.h"
#include "app_gpiote.c"
#include "app_timer.h"
#include "softdevice_handler.h"

#define APP_GPIOTE_MAX_USERS	1
#define BUTTON_0	3

app_gpiote_user_id_t m_app_gpiote_my_id;

void gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
    if (event_pins_high_to_low & (1 << BUTTON_0)){
        nrf_gpio_pin_set(LED_0);
    	nrf_gpio_pin_set(2);
    }
    
    if (event_pins_low_to_high & (1 << BUTTON_0)){
        nrf_gpio_pin_clear(LED_0);
    	nrf_gpio_pin_clear(2);
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
	nrf_gpio_cfg_output(2);
	nrf_gpio_cfg_input(3, NRF_GPIO_PIN_NOPULL);

	APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
	err_code = app_gpiote_user_register(&m_app_gpiote_my_id, (1 << BUTTON_0), (1 << BUTTON_0), gpiote_event_handler);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_gpiote_user_enable(m_app_gpiote_my_id);
    APP_ERROR_CHECK(err_code);
}

int main(void){

    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);

	configureGPIO();
	
	uint8_t inputRead;
//	uint32_t x;
//	app_timer_cnt_get(&x);
	while(1){
		// inputRead = nrf_gpio_port_read(3);
		// if(inputRead) nrf_gpio_pin_set(2);
		// else nrf_gpio_pin_clear(2);
		// nrf_delay_us(1);
		sd_app_evt_wait();
	}
}


