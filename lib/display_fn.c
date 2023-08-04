/*
 * display_fn.c
 *
 * Created: 21.06.2023 23:43:21
 *  Author: princ
 */ 
#include "display_fn.h"
#include <u8g2.h>



#define DISPLAY_CLK_DIR DDRC
#define DISPLAY_CLK_PORT PORTC
#define DISPLAY_CLK_PIN 2

#define DISPLAY_DATA_DIR DDRC
#define DISPLAY_DATA_PORT PORTC
#define DISPLAY_DATA_PIN 3

#define DISPLAY_CS_DIR DDRE
#define DISPLAY_CS_PORT PORTE
#define DISPLAY_CS_PIN 3

#define DISPLAY_DC_DIR DDRC
#define DISPLAY_DC_PORT PORTC
#define DISPLAY_DC_PIN 1

#define DISPLAY_RESET_DIR DDRC
#define DISPLAY_RESET_PORT PORTC
#define DISPLAY_RESET_PIN 0







uint8_t u8x8_avr_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	// Re-use library for delays

	switch(msg)
	{
		case U8X8_MSG_GPIO_AND_DELAY_INIT:  // called once during init phase of u8g2/u8x8
		DISPLAY_CLK_DIR |= 1<<DISPLAY_CLK_PIN;
		DISPLAY_DATA_DIR |= 1<<DISPLAY_DATA_PIN;
		DISPLAY_CS_DIR |= 1<<DISPLAY_CS_PIN;
		DISPLAY_DC_DIR |= 1<<DISPLAY_DC_PIN;
		DISPLAY_RESET_DIR |= 1<<DISPLAY_RESET_PIN;
		
		break;              // can be used to setup pins
		case U8X8_MSG_GPIO_SPI_CLOCK:        // Clock pin: Output level in arg_int
		if(arg_int)
		DISPLAY_CLK_PORT |= (1<<DISPLAY_CLK_PIN);
		else
		DISPLAY_CLK_PORT &= ~(1<<DISPLAY_CLK_PIN);
		break;
		case U8X8_MSG_GPIO_SPI_DATA:        // MOSI pin: Output level in arg_int
		if(arg_int)
		DISPLAY_DATA_PORT |= (1<<DISPLAY_DATA_PIN);
		else
		DISPLAY_DATA_PORT &= ~(1<<DISPLAY_DATA_PIN);
		break;
		case U8X8_MSG_GPIO_CS:        // CS (chip select) pin: Output level in arg_int
		if(arg_int)
		DISPLAY_CS_PORT |= (1<<DISPLAY_CS_PIN);
		else
		DISPLAY_CS_PORT &= ~(1<<DISPLAY_CS_PIN);
		break;
		case U8X8_MSG_GPIO_DC:        // DC (data/cmd, A0, register select) pin: Output level in arg_int
		if(arg_int)
		DISPLAY_DC_PORT |= (1<<DISPLAY_DC_PIN);
		else
		DISPLAY_DC_PORT &= ~(1<<DISPLAY_DC_PIN);
		break;
		
		case U8X8_MSG_GPIO_RESET:     // Reset pin: Output level in arg_int
		if(arg_int)
		DISPLAY_RESET_PORT |= (1<<DISPLAY_RESET_PIN);
		else
		DISPLAY_RESET_PORT &= ~(1<<DISPLAY_RESET_PIN);
		break;
		default:
		return 1;
		u8x8_SetGPIOResult(u8x8, 1);      // default return value
		break;
	}
	return 1;
}