// LED Library

// Assumes active low LEDs

#include <stdint.h>
#include "nrf_gpio.h"

void led_init (uint32_t pin_number);
void led_on (uint32_t pin_number);
void led_off (uint32_t pin_number);
void led_toggle (uint32_t pin_number);
void gpio_init(uint32_t pin_number);
uint32_t gpio_read(uint32_t pin_number);
