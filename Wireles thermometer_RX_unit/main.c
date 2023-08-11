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
#include "wdt_hal.h"

#include "string.h"
#include "float.h"
#include "stdbool.h"
#include <stdio.h>
//04.1f
gpio health = {(uint8_t *)&PORTB , PORTB5};
gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};
gpio RF_mode = {(uint8_t *)&PORTD , PORTD4};
gpio buzer = {(uint8_t *)&PORTB , PORTB1};


static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);
u8g2_t u8g2;
uint8_t serial_buff[128];
uint8_t display_line[64];
uint16_t att = 0;

uint16_t  RFID=1298;  //1298
uint16_t  DVID=4517;  //4517
uint16_t  RFC=10;
uint16_t  BAUD=4;


uint16_t param_value1;
uint16_t param_value2;


int main(void)
{
	
	WDT_enable();
	WDT_prescaler_change(0, wdt_timeout_1s);
   
    
    stdout = &mystdout;
   
    sei();
    
    
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
    
    set_pin_dir(&lcd_blk , PORT_DIR_OUT);
    set_pin_dir(&RF_mode , PORT_DIR_OUT); set_pin_level(&RF_mode, true);
	set_pin_dir(&buzer , PORT_DIR_OUT); set_pin_level(&buzer, false);
    
    
    u8g2_ClearBuffer(&u8g2);
    //u8g2_SetFont(&u8g2, u8g2_font_5x8_t_cyrillic);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
    u8g2_DrawStr(&u8g2, 1, 10, (void *)"RX MODULE");
	
	sprintf(display_line, "RFID-%04u", RFID);
	u8g2_DrawStr(&u8g2, 1, 20, (void *)display_line);
    sprintf(display_line, "DVID-%04u\n; RFC-%03u",DVID);
    u8g2_DrawStr(&u8g2, 1, 30, (void *)display_line);
	sprintf(display_line, "RFC-%03u", RFC);
	u8g2_DrawStr(&u8g2, 1, 40, (void *)display_line);
	u8g2_DrawStr(&u8g2, 1, 50, (void *)"Watchdog run" );
	u8g2_SendBuffer(&u8g2);
	
	set_pin_level(&RF_mode, false);
	_delay_ms(500);
	//printf("AT+CLSS\r\n");
	//_delay_ms(200);
	//printf("AT+RFID\r\n");
	//_delay_ms(200);
	//printf("AT+DVID\r\n");
	//_delay_ms(200);
	//printf("AT+RFC\r\n");
	//_delay_ms(200);
	//printf("AT+POWE\r\n");
	//_delay_ms(200);
	
	//wdr();
	printf("AT+RFID%04u\r\n" , (uint16_t *)RFID);
	_delay_ms(500);
	printf("AT+DVID%04u\r\n" , (uint16_t *)DVID);
	_delay_ms(500);
	printf("AT+RFC%03u\r\n" , (uint16_t *)RFC);
	_delay_ms(500);
	set_pin_level(&RF_mode, true);
	_delay_ms(500);
	u8g2_ClearBuffer(&u8g2);
	u8g2_SendBuffer(&u8g2);
	
	
   wdr();
    double temp = 0.0f;
	bool newDataAvailable = true;
	bool TxResponseDataValid = false;
	bool overheatDetect = false;
    uint16_t tempRaw = 0;
	uint16_t dataValidCounter = 0;
	 set_pin_level(&lcd_blk, true);
	while (1) 
    {
		//wdr();
		if(newDataAvailable){
			
			if(TxResponseDataValid){
			if (tempRaw>999){
				sprintf(display_line , "%03d ", (uint16_t)temp);
				}else{
				sprintf(display_line , "%04.1f", temp);
			}
			u8g2_SetFont(&u8g2, u8g2_font_spleen32x64_mf);
			u8g2_ClearBuffer(&u8g2);
			u8g2_DrawStr(&u8g2, 1, 52, (uint8_t *)display_line);
			
			if (TxResponseDataValid){
				sprintf(charArray, "%04u", dataValidCounter);
			}else{
				sprintf(charArray, "ERROR");
			}
			
			u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
			u8g2_DrawStr(&u8g2, 1, 10, (uint8_t *)charArray);
			u8g2_SendBuffer(&u8g2);
			newDataAvailable = false;
			wdr();
			}else{
				
				sprintf(display_line , "ERR!");
				u8g2_SetFont(&u8g2, u8g2_font_spleen32x64_mf);
				u8g2_ClearBuffer(&u8g2);
				u8g2_DrawStr(&u8g2, 1, 52, (uint8_t *)display_line);
				u8g2_SendBuffer(&u8g2);
				
			}}
		
		
		if (serial_complete()){
			uint8_t *data_p = (uint8_t *)serial_read_data();
			param_value1 = extractValue(data_p, 1);
			param_value2 = extractValue(data_p, 2);
			if(param_value1 = param_value2){
				if (param_value1 > 850){
					overheatDetect = true;
				}else{
					overheatDetect = false;
				}
				temp = param_value1*0.1;
				tempRaw = param_value1;
				newDataAvailable = true;
				TxResponseDataValid = true;
				dataValidCounter = 0;
			}
			
		}
	if (dataValidCounter < 240){
		dataValidCounter++;
	}else{
		TxResponseDataValid = false;
		newDataAvailable = true;
		dataValidCounter = 0;
	}
	
	if(overheatDetect == true || TxResponseDataValid == false){
		toggle_pin_level(&lcd_blk);
		toggle_pin_level(&buzer);
		
	}else{
		set_pin_level(&lcd_blk, true);
		set_pin_level(&buzer, false);
	}
	
	
	_delay_ms(50);	
    }
}

