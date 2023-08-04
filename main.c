/*
 * Wireles_thermometer.c
 *
 * Created: 25.07.2023 11:20:49
 * Author : princ
 */ 
#include "config.h"
#include <avr/io.h>
#include <u8g2.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "lib/display_fn.h"
#include "lib/uart_hal.h"
#include "lib/gpio_driver.h"
#include "lib/twi_hal.h"
#include "lib/rtc.h"
#include "lib/ds18S20.h"
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
	TSDS18x20 DS18x20;
	TSDS18x20 *pDS18x20 = &DS18x20;
	
	
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
	
	
	if (DS18x20_Init(pDS18x20,&PORTB,1))
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
		u8g2_DrawStr(&u8g2, 1, 10, (char *)"DS18B20 ERROR");
		u8g2_SendBuffer(&u8g2);
		while(true){}
	}
	DS18x20_SetResolution(pDS18x20,CONF_RES_10b);
	DS18x20_WriteScratchpad(pDS18x20);
	
	
	u8g2_SendBuffer(&u8g2);
    double temp = 0.0f;
	
	while (1) 
    {	
		
		
		
		if (DS18x20_MeasureTemperature(pDS18x20))
		{
			//_delay_ms(200);
			temp=DS18x20_TemperatureValue(pDS18x20)*10;
			printf("val:%04x:%04x:e\n\r" , (uint16_t)temp, (uint16_t)temp); //For checking after receiving in remote module
			temp = temp*0.1;
			if (att>999){
				sprintf(charArray , "%03d ", (uint16_t)temp);
			}else{
				sprintf(charArray , "%04.1f", temp);
			}
			u8g2_ClearBuffer(&u8g2);
			u8g2_SetFont(&u8g2, u8g2_font_spleen32x64_mf);
			u8g2_DrawStr(&u8g2, 1, 50, (char *)charArray);
			u8g2_SendBuffer(&u8g2);
		}else{
			u8g2_ClearBuffer(&u8g2);
			u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
			u8g2_DrawStr(&u8g2, 1, 10, (char *)"DS18B20 CRC ERROR");
			u8g2_DrawStr(&u8g2, 1, 20, (char *)"TRU TO INIT AGAIN");
			u8g2_SendBuffer(&u8g2);
			printf("val:ffff:ffff:e\n\r");
			
			if (DS18x20_Init(pDS18x20,&PORTB,1))
			{
				u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
				u8g2_DrawStr(&u8g2, 1, 30, (char *)"SENSOR CAN'T BE FIND ");
				u8g2_SendBuffer(&u8g2);
				}else{
				DS18x20_SetResolution(pDS18x20,CONF_RES_10b);
				DS18x20_WriteScratchpad(pDS18x20);
			}
			_delay_ms(200);
		}
		if (serial_complete()){
			uint8_t const *data_p = (void *)serial_read_data();
			
			uint8_t val = parseString((void *)data_p);
			if (val < 32){
				att = val;
			}else{
				att = 31;
			}
			u8g2_DrawStr(&u8g2, 1, 8, (void*)data_p);
			u8g2_SendBuffer(&u8g2);
		}
    }
}

