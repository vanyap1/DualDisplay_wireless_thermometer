/*
 * adc.c
 *
 * Created: 03.02.2023 19:01:00
 *  Author: User
 */ 
#include "adc_hal.h"

volatile static uint8_t adc_convert_done = 1;

ISR(ADC_vect){
	adc_convert_done = 1;
}

void adc_init(void){
	ADMUX |= REFS_1_1V << REFS0 | 1 << REFS1;
	ADCSRA |= 1 << ADEN | 1 << ADIE | 0b111 << ADPS0;
}


void adc_pin_enable(uint8_t pin){
	DIDR0 |= 1 << pin;
}

void adc_pin_disable(uint8_t pin){
	DIDR0 &= ~(1 << pin);
}

void adc_pin_select(uint8_t souce){
	ADMUX &= 0xF0;
	ADMUX |= souce;
}


uint16_t adc_convert(void){
	ADCSRA |= 1 << ADEN;
	uint8_t adcl = 0;
	uint8_t adch = 0;
	
	adc_convert_done = 0;
	
	ADCSRA |= 1 << ADSC;
	while(adc_convert_done == 0);
	
	adcl = ADCL;
	adch = ADCH;
	ADCSRA &=~ (1 << ADEN);
	return (adch << 8 | adcl);
	
	
}

uint16_t get_adc(uint8_t source){
	adc_pin_select(source);
	//uint16_t adc_tmp_data = adc_convert();
	return adc_convert();
}

uint16_t get_mVolt(uint8_t source){
	adc_pin_select(source);
	//uint16_t adc_tmp_data = adc_convert();
	return ADC_VOLT(adc_convert());
}







uint16_t voltageMovingAverage(uint16_t measurement) {
	v_filter[v_currentIndex] = measurement;
	v_currentIndex = (v_currentIndex + 1) % V_FILTER_SIZE;

	uint32_t sum = 0;
	for (int i = 0; i < V_FILTER_SIZE; i++) {
		sum += v_filter[i];
	}
	return sum / V_FILTER_SIZE;
}

uint16_t currentMovingAverage(uint16_t measurement) {
	i_filter[i_currentIndex] = measurement;
	i_currentIndex = (i_currentIndex + 1) % I_FILTER_SIZE;

	uint32_t sum = 0;
	for (int i = 0; i < I_FILTER_SIZE; i++) {
		sum += i_filter[i];
	}
	return sum / I_FILTER_SIZE;
}

uint16_t filterMovingAverage(uint16_t measurement, int channel) {
	filter[channel][currentIndex[channel]] = measurement;
	currentIndex[channel] = (currentIndex[channel] + 1) % FILTER_SIZE;

	uint32_t sum = 0;
	for (int i = 0; i < FILTER_SIZE; i++) {
		sum += filter[channel][i];
	}

	return sum / FILTER_SIZE;
}