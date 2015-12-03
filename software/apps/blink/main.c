/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "led.h"

void app_error_handler(uint32_t error_code,
                       uint32_t line_num,
                       const uint8_t * p_file_name) {
    NVIC_SystemReset();
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    platform_init();

    // Configure LED-pins as outputs.

    // Toggle LEDs.
    while (true)
    {
        for (int i = 0; i < 3; i++)
        {
            led_toggle(LED_0);
        }
    }
}


/** @} */
