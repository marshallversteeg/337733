/*
 * Send an advertisement periodically
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "ble_advdata.h"
#include "boards.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_debug_assert_handler.h"
#include "led.h"

 #include <stdio.h>

#include "simple_ble.h"
#include "simple_adv.h"


// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x80,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};


int main(void) {
	platform_init();
    uint32_t err_code;

    led_init(LED_0);

    //LED stuff
    led_init(LED_1);
    gpio_init(2);

    // Setup BLE
    simple_ble_init(&ble_config);
    simple_adv_only_name(gpio_read(2));

/*
    //led_on(LED_0);
    while (true) {
        if(gpio_read(2) > 0){
            led_on(LED_1);
            //for(unsigned int i = 0; i < 1000000; i++){}
        }
        else{
            led_off(LED_1);
        }

	    // Advertise because why not

     //   power_manage();
    }
*/
}
