/*
 * I2C.c
 *
 * Created: 5/02/2026 11:21:03
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"
#include <util/delay.h>
#define slave1 0x30;
#define slave2 0x40
#define slave1R (0x30 << 1) | 0x01
#define slave1w (0x30 << 1) & 0b11111110

uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C = 0;
/****************************************/
// Function prototypes
void refreshPORT(uint8_t valor);
void setup(void);
/****************************************/
// Main Function
int main(void)
{
	setup();
	I2C_master_init(100000, 1); //inicializamos a una frecuencia de 100KHz
	while (1)
	{
		PORTB |= (1<<PORTB5);
		
		if(!I2C_master_start()){
			 return;
		}
		if(!master_write(slave1w)){
			I2C_master_stop();
			return;
		}
		master_write('R');
		
		if (!I2C_master_Reapetedstart()){
			I2C_master_stop();
			return;
		}
		
		if(!master_write(slave1R)){
			I2C_master_stop();
			return;
		}
		
		I2C_master_read(&bufferI2C, 0);
		I2C_master_stop();
		
		PORTB &= ~(1<<PORTB5);
		_delay_ms(1000);
	}
}
/****************************************/
// NON-Interrupt subroutines
void refreshPORT(uint8_t valor){
	if(valor & 0b10000000){
		PORTB |= (1<<PORTB1);
	}
	else{
		PORTB &= ~(1<<PORTB1);
	}
	if(valor & 0b01000000){
		PORTB |= (1<<PORTB0);
	}
	else{
		PORTB &= ~(1<<PORTB0);
	}
	if(valor & 0b00100000){
		PORTD |= (1<<PORTD7);
	}
	else{
		PORTD &= ~(1<<PORTD7);
	}
	if(valor & 0b00010000){
		PORTD |= (1<<PORTD6);
	}
	else{
		PORTD &= ~(1<<PORTD6);
	}
	if(valor & 0b00001000){
		PORTD |= (1<<PORTD5);
	}
	else{
		PORTD &= ~(1<<PORTD5);
	}
	if(valor & 0b00000100){
		PORTD |= (1<<PORTD4);
	}
	else{
		PORTD &= ~(1<<PORTD4);
	}
	if(valor & 0b00000010){
		PORTD |= (1<<PORTD3);
	}
	else{
		PORTD &= ~(1<<PORTD3);
	}
	if(valor & 0b00000001){
		PORTD |= (1<<PORTD2);
	}
	else{
		PORTD &= ~(1<<PORTD2);
	}
	
}
void setup(){
	//puertos de salida para los 8 leds
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	DDRB |= (1<<DDB0)|(1<<DDB1);
	//puerto led
	DDRB |= (1<<DDB5);
	//iniciando leds en 0
	PORTB &= ~(1<<DDB5);
	
	//Limpiamos puertos leds = 0
	
	PORTB &= ~((1<< PORTB0)|(1<<PORTB1));
	PORTD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7));
}	

/****************************************/
// Interrupt routines
