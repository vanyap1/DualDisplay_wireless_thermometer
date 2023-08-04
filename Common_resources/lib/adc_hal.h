/*
 * adc.h
 *
 * Created: 03.02.2023 19:01:11
 *  Author: User
 */ 
#ifndef ADC_HAL_H_
#define ADC_HAL_H_

//#include <stdint.h>
#include "config.h"
//#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>


#endif /* ADC_HAL_H_ */

#define ADC_VOLT(x) (x * 1.188440)
#define REFS_1_1V	1
#define REFS_AVCC	0


#define V_FILTER_SIZE 10
uint16_t v_filter[V_FILTER_SIZE];
uint8_t v_currentIndex;

#define I_FILTER_SIZE 3
uint16_t i_filter[I_FILTER_SIZE];
uint8_t i_currentIndex;


#define FILTER_SIZE 10
#define NUM_CHANNELS 4

uint16_t filter[NUM_CHANNELS][FILTER_SIZE];
int currentIndex[NUM_CHANNELS];


enum{
	ADC0_PIN,
	ADC1_PIN,
	ADC2_PIN,
	ADC3_PIN,
	ADC4_PIN,
	ADC5_PIN,
	ADC6_PIN,
	ADC7_PIN,
	ADC8_TEMPERATURE,
	ADC_1V1 = 0b1110,
	ADC_GND,
};


void adc_init(void);
void adc_pin_enable(uint8_t pin);
void adc_pin_disable(uint8_t pin);
void adc_pin_select(uint8_t souce);
uint16_t adc_convert(void);
uint16_t get_adc(uint8_t source);
uint16_t get_mVolt(uint8_t source);

uint16_t voltageMovingAverage(uint16_t measurement);
uint16_t currentMovingAverage(uint16_t measurement);
uint16_t filterMovingAverage(uint16_t measurement, int channel);