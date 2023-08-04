/*
 * display_fn.h
 *
 * Created: 21.06.2023 23:45:13
 *  Author: princ
 */ 

#include <u8g2.h>
#include <util/delay.h>

uint8_t u8x8_avr_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);