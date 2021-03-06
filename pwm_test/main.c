/* Send out a 10micro second pulse every 100ms */

#include <stdbool.h>
#include <stdint.h>
#include "app_pwm.h"
#include "led.h"
#include "app_gpiote.h"
#include "boards.h"
#include "nordic_common.h"
#include "app_timer.h"
#include "softdevice_handler.h"

APP_PWM_INSTANCE(PWM1,1);                   // Create the instance "PWM1" using TIMER1.
void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    
}

ret_code_t err_code;
/* 2-channel PWM, 200 Hz, output on DK LED pins. */
app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, BSP_LED_0, BSP_LED_1);
/* Switch the polarity of the second channel. */
pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
/* Initialize and enable PWM. */
err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
APP_ERROR_CHECK(err_code);
app_pwm_enable(&PWM1);
uint32_t value;
while(true)
{
    for (uint8_t i = 0; i < 40; ++i)
    {
        value = (i < 20) ? (i * 5) : (100 - (i - 20) * 5);
        
        /* Se the duty cycle - keep trying until PWM is ready. */
        while (app_pwm_channel_duty_set(&PWM1, 0, value) == NRF_ERROR_BUSY);
        while (app_pwm_channel_duty_set(&PWM1, 1, value) == NRF_ERROR_BUSY);
        nrf_delay_ms(25);
    }
}

