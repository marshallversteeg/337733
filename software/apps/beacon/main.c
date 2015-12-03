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
#include "ble_bas.h"

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

	//ble_bas_t *p = malloc(sizeof(ble_bas_t));
	//const ble_bas_init_t *q = malloc(sizeof(ble_bas_init_t));

	//p->service_handle = 0x1111;
	//q->initial_batt_level = 0x63;

	simple_ble_app_t* app = simple_ble_init(&ble_config);

	#define SHORT_UUID 0x1234
	const ble_uuid128_t my_uuid128 = {
		{0x11, 0xbb, 0x5e, 0x39, 0x31, 0x52, 0x45, 0x0c,
			0x90, 0xee, 0x3f, 0xa2, 0x9c, 0x86, 0x8c, 0xd6}
	};

	ble_uuid_t my_uuid;
	app.service_handle = simple_ble_add_service(&my_uuid128, &my_uuid, SHORT_UUID);

	simple_ble_add_characteristic(1,1,1,
									my_uuid.type,
									CHAR_SHORT_UUID,
									1, app.char_memory,
									app.service_handle,
									&app.char_handle);


    uint32_t err_code;

    led_init(LED_0);

    // Setup BLE
    //simple_ble_init(&ble_config);

    // Advertise because why not
    simple_adv_only_name();

    led_on(LED_0);

    //ble_bas_init(p, q);


    while (1) {
        power_manage();
    }
}
