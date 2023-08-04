/*
 * Wireles thermometer_RX_unit.c
 *
 * Created: 04.08.2023 23:28:37
 * Author : Vanya
 */ 

#include "config.h"
#include <avr/io.h>
#include <u8g2.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "display_fn.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal.h"
#include "rtc.h"
#include "ds18S20.h"
#include "string.h"
#include "float.h"
#include "stdbool.h"
#include <stdio.h>
//04.1f
gpio health = {(uint8_t *)&PORTB , PORTB5};
gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};
gpio RF_mode = {(uint8_t *)&PORTD , PORTD4};

static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);
u8g2_t u8g2;
uint8_t serial_buff[128];
uint16_t att = 0;


int main(void)
{
	
    sei();
    stdout = &mystdout;
    
    
    
    char charArray[64] = "test string";
    u8g2_Setup_st7565_zolen_128x64_f( &u8g2, U8G2_MIRROR_VERTICAL, u8x8_byte_4wire_sw_spi, u8x8_avr_gpio_and_delay);
    // u8x8_byte_4wire_sw_spi located there - u8x8_byte.c
    
    uart_init(9600,0);
    
    //twi_init(400000);
    //rtc_set(&sys_rtc);
    
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_SetFlipMode(&u8g2, 1);
    u8g2_SetContrast(&u8g2, 120);
    
    set_pin_dir(&lcd_blk , PORT_DIR_OUT); set_pin_level(&lcd_blk, true);
    set_pin_dir(&RF_mode , PORT_DIR_OUT); set_pin_level(&RF_mode, true);
    
    
    u8g2_ClearBuffer(&u8g2);
    //u8g2_SetFont(&u8g2, u8g2_font_5x8_t_cyrillic);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
    
  
    u8g2_SendBuffer(&u8g2);
    double temp = 0.0f;
    while (1) 
    {
    }
}

