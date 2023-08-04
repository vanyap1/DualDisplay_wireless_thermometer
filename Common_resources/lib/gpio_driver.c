
#include "gpio_driver.h"




/*
int = 2;
do {
	
	
	set_pin_level(&dev3_ld_grn, false);
	_delay_ms(100);
	set_pin_level(&dev3_ld_grn, true);
	_delay_ms(50);
} while (--i);

*/


void set_pin_level(gpio *GPIOx , const bool level)
{
	if (level) {
		*GPIOx->port |= 1 << GPIOx->pin;
		} else {
		*GPIOx->port &= ~(1 << GPIOx->pin);
	}
}

void set_pin_dir(gpio *GPIOx, const enum port_dir direction)
{
	uint8_t *tmp_port = GPIOx->port-1;
	switch (direction) {
		case PORT_DIR_IN:
		*tmp_port &= ~(1 << GPIOx->pin);
		break;
		case PORT_DIR_OUT:
		*tmp_port |= 1 << GPIOx->pin;
		break;
		case PORT_DIR_OFF:
		*tmp_port &= ~(1 << GPIOx->pin);
		*GPIOx->port &= ~(1 << GPIOx->pin);
		break;
		default:
		break;
	}
}

void set_pin_pull_mode(gpio *GPIOx, const enum port_pull_mode pull_mode)
{
	uint8_t *tmp_port = GPIOx->port-1;
	if (pull_mode == PORT_PULL_UP) {

		*tmp_port &= ~(1 << GPIOx->pin);

		*GPIOx->port |= 1 << GPIOx->pin;
		} else if (pull_mode == PORT_PULL_OFF) {

		*GPIOx->port &= ~(1 << GPIOx->pin);
	}
}

void set_port_dir(gpio *GPIOx, const uint8_t mask)
{
	uint8_t *tmp_port = GPIOx->port-1;
	*tmp_port=mask;

}


void set_port_level(gpio *GPIOx, const uint8_t mask){
	*GPIOx->port=mask;
}

void toggle_pin_level(gpio *GPIOx)
{
	uint8_t *tmp_port = GPIOx->port-2;
	if (*tmp_port & (1 << GPIOx->pin)) {
		*GPIOx->port &= ~(1 << GPIOx->pin);
		} else {
		*GPIOx->port |= 1 << GPIOx->pin;
	}	
}

void set_port_direction(gpio_port *GPIOx, const uint8_t mask, const uint8_t init_value){
	uint8_t *tmp_port = GPIOx->port-1;
	*tmp_port=mask;
	uint8_t *default_value = GPIOx->port;
	*default_value=~init_value;
	
}
void set_port_data(gpio_port *GPIOx, const uint8_t mask){
	uint8_t *tmp_port = GPIOx->port;
	*tmp_port=~mask;
}

void set_port_pin_level(gpio_port *GPIOx, const uint8_t pin, const uint8_t level){
	if (level) {
		*GPIOx->port |= 1 << pin;
		} else {
		*GPIOx->port &= ~(1 << pin);
	}
}

void toggle_port_pin_level(gpio_port *GPIOx, const uint8_t pin){
	uint8_t *tmp_port = GPIOx->port-2;
	if (*tmp_port & (1 << pin)) {
		*GPIOx->port &= ~(1 << pin);
		} else {
		*GPIOx->port |= 1 << pin;
	}
	
}