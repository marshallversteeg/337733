// LED Library

// Assumes active low LEDs

#include <stdint.h>
#include "led.h"
#include "nrf_gpio.h"


void led_init (uint32_t pin_number) {
	nrf_gpio_cfg_output(pin_number);
    nrf_gpio_pin_set(pin_number);
}

void led_on (uint32_t pin_number) {
    nrf_gpio_pin_clear(pin_number);
}

void led_off (uint32_t pin_number) {
    nrf_gpio_pin_set(pin_number);
}

void led_toggle (uint32_t pin_number) {
    nrf_gpio_pin_toggle(pin_number);
}

void gpio_init(uint32_t pin_number){
	nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_pin_set(pin_number);
}

uint32_t gpio_read(uint32_t pin_number){
	return nrf_gpio_pin_read(pin_number);
}