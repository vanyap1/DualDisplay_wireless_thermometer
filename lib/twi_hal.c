/*
 * twi_hal.c
 *
 * Created: 2020/11/01 18:07:03
 *  Author: DevilBinder
 */ 


/* Code EXAMPLES

Print error - 

for (i = 1; i < 128; ++i){
	err = twi_wire(i,0x00,rtc_config_3,1);
	if(err == TWI_OK){
		memset(char_array,0,sizeof(char_array));
		sprintf(char_array,"%d error %d, i2c addr %02x\n\r",__LINE__,err, i);
		uart_send_string((uint8_t*)char_array);
		//while(1);
	}}
*/


#include "twi_hal.h"

volatile uint8_t status = 0xF8;

ISR(TWI1_vect){
	status = (TWSR1 & 0xF8);
}

static uint8_t twi_start(void){
	uint16_t i = 0;
	TWCR1 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	while(status != TWI_START){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}

static void twi_stop(void){
	
	TWCR1 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
}

static uint8_t twi_restart(void){
	uint16_t i = 0;
	TWCR1 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	while(status != TWI_RSTART){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}

static uint8_t twi_addr_write_ack(void){
	uint16_t i = 0;
	
	TWCR1 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIT_ADDR_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi_data_write_ack(void){
	uint16_t i = 0;
	
	TWCR1 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIT_DATA_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi_addr_read_ack(void){
	uint16_t i = 0;
	
	TWCR1 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIR_ADDR_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi_data_read_ack(uint8_t ack){
	uint16_t i = 0;
	if(ack != 0){
		TWCR1 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		while(status != TWIR_DATA_ACK){
			i++;
			if(i >= TWI_TIMEOUT){
				return TWI_ERROR_START;
			}
		}
	}else{
		
		TWCR1 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		while(status != TWIR_DATA_NACK){
			i++;
			if(i >= TWI_TIMEOUT){
				return TWI_ERROR_START;
			}
		}
		
	}
	return TWI_OK;
}

uint8_t twi_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi_start();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	TWDR1 = (addr << 1) | 0;
	
	
	err = twi_addr_write_ack();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	TWDR1 = reg;
	err = twi_data_write_ack();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	err = twi_restart();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	TWDR1 = (addr << 1) | 1;

	err = twi_addr_read_ack();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	for(i = 0; i < (len - 1);i++){
		err = twi_data_read_ack(1);
		if(err != TWI_OK){
			twi_stop();
			return err;
		}
		data[i] = TWDR1;
	}
	
	err = twi_data_read_ack(0);
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	data[i] = TWDR1;
	
	twi_stop();
	
	return err;
}


uint8_t twi_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi_start();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	TWDR1 = (addr << 1) | 0;
	
	
	err = twi_addr_write_ack();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	TWDR1 = reg;
	err = twi_data_write_ack();
	if(err != TWI_OK){
		twi_stop();
		return err;
	}
	
	for(i = 0; i < len;i++){
		TWDR1 = data[i];
		err = twi_data_write_ack();
		if(err != TWI_OK){
			twi_stop();
			return err;
		}
	}
	
	
	twi_stop();
	
	return err;
}





void twi_init(uint32_t speed){
	
	uint32_t gen_t = 0;
	gen_t = (((F_CPU/speed) - 16) / 2) & 0xFF;
	TWBR1 = gen_t & 0xFF;
	TWCR1 = (1 << TWEN) | (1 << TWIE);
	//PORTC |= (1 << PORTC5 | 1 << PORTC4);
	
}


