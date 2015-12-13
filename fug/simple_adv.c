/*
 * Advertise just a device name.
 */

// Standard Libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Nordic Libraries
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "ble.h"
#include "ble_db_discovery.h"
#include "app_util.h"
#include "app_error.h"
#include "ble_advdata_parser.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "app_trace.h"
#include "ble_hrs_c.h"
#include "ble_bas_c.h"
#include "app_util.h"

// Platform, Peripherals, Devices, Services
#include "simple_ble.h"
#include "simple_adv.h"

void simple_adv_only_name (uint32_t input) {
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;


    uint32_t value = input;
    uint8_t adv[4];

    adv[3] = (value & 0x000f);
    adv[2] = (value & 0x00f0) >> 8;
    adv[1] = (value & 0x0f00) >> 16;
    adv[0] = (value & 0xf000) >> 24;


    //const static uint8_t adv[] = {0x01,0x02,0x03,0x04,0x05};
    ble_advdata_manuf_data_t manuf;
    manuf.company_identifier = 0x0123;
    manuf.data.size = 0x04;
    manuf.data.p_data = adv;
    advdata.p_manuf_specific_data = &manuf;


/*
    uint8_array_t derp;
    derp.size = 0x01;
    uint8_t abc = 0x0001;
    uint8_t* datac = &abc;
    derp.p_data = datac;
    advdata.p_manuf_specific_data->data = derp;
*/

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Start the advertisement
    advertising_start();
}

void simple_adv_service (ble_uuid_t* service_uuid) {
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    memset(&srdata, 0, sizeof(srdata));

    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = 1;
    advdata.uuids_complete.p_uuids  = service_uuid;

    // Put the name in the SCAN RESPONSE data
    srdata.name_type                = BLE_ADVDATA_FULL_NAME;

    err_code = ble_advdata_set(&advdata, &srdata);
    APP_ERROR_CHECK(err_code);

    // Start the advertisement
    advertising_start();
}
