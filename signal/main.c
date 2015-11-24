/* Send out a 10micro second pulse every 100ms */

#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "app_gpiote.h"
#include "boards.h"
#include "nordic_common.h"
#include "app_timer.h"
#include "softdevice_handler.h"

// Some constants about timers
#define SIGNAL_TIMER_PRESCALER             0
/**< Value of the RTC1 PRESCALER register. */
#define SIGNAL_TIMER_MAX_TIMERS             4
/**< Maximum number of simultaneously created timers. */
#define SIGNAL_TIMER_OP_QUEUE_SIZE         12
/**< Size of timer operation queues. */

// How long before the timer fires.
#define SIGNAL_ON_1     APP_TIMER_TICKS(320, SIGNAL_TIMER_PRESCALER)
#define SIGNAL_OFF_2     APP_TIMER_TICKS(10, SIGNAL_TIMER_PRESCALER)

// Timer data structure
static app_timer_id_t  signal_timer1;                    
static app_timer_id_t  signal_timer2;        
            
// Handlers
static void timer_handler2 (void* p_context);
uint32_t signal;

//#define MAX_USERS	1
//APP_GPIOTE_INIT(MAX_USERS);

void app_error_handler(uint32_t error_code,
                       uint32_t line_num,
                       const uint8_t * p_file_name) {
    NVIC_SystemReset();
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
    app_error_handler(0x11, line_num, p_file_name);
}

// Timer callback
static void timer_handler1 (void* p_context) {
	nrf_gpio_pin_set(2);
    led_toggle(LED_0);
    uint32_t err_code;
    err_code = app_timer_create(&signal_timer2,
                                APP_TIMER_MODE_REPEATED,
                                timer_handler2);
    APP_ERROR_CHECK(err_code);
}

// Timer callback
static void timer_handler2 (void* p_context) {
	nrf_gpio_pin_clear(2);
    led_off(LED_0);
}

// Timer callback
static void timer_handler3 (void* p_context) {
    led_toggle(LED_2);
}

// Setup timer
static void timer_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(SIGNAL_TIMER_PRESCALER,
                   SIGNAL_TIMER_MAX_TIMERS,
                   SIGNAL_TIMER_OP_QUEUE_SIZE,
                   false);

    // Create a timer
    err_code = app_timer_create(&signal_timer1,
                                APP_TIMER_MODE_REPEATED,
                                timer_handler1);
    APP_ERROR_CHECK(err_code);
}

// Start the blink timer
static void timer_start(void) {
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(signal_timer1, SIGNAL_ON_1, NULL);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(signal_timer2, SIGNAL_OFF_2, NULL);
    APP_ERROR_CHECK(err_code);
}

int main(void) {

    // Initialize.
    platform_init();

    // Need to set the clock to something
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);

	nrf_gpio_cfg_output(2);

    timer_init();
    timer_start();
	//led_toggle(LED_0);
    // Enter main loop.
    while (1) {
        sd_app_evt_wait();
    }
}
