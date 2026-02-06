/*
 * SLAVE_I2C.c
 *
 * Created: 5/02/2026 19:17:54
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#define Slaveadress 0x30
#include "I2C/I2C.h"
#include "ADC/ADC.h"
uint8_t MULTIPLEXADO;
/****************************************/
// Function prototypes
uint8_t buffer = 0;
volatile uint8_t valorADC = 0;
/****************************************/
// Main Function
int main(void)
{
	DDRB |= (1<<DDB5);
	PORTB &= ~(1<<PORTB5);
	init_ADC(1, 128, 0);
	I2C_slave_init(Slaveadress);
	sei();
	while (1)
	{
		if(buffer == 'R'){
			PINB |= (1<<PINB5);
			buffer = 0;
		}
		
	}
}
/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines
ISR(TWI_vect){
	uint8_t estado = TWSR & 0x0F;
	switch(estado){
		//slave debe recibir dato
		case 0x60:
		case 0x70:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		break;
		
	    case 0x80:
		case 0x90:
			buffer = TWDR;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
			break;
			
		case 0xA8:
		case 0xB8:
			TWDR = valorADC;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		break;
		
		case 0xC0:
		case 0XC8:
			TWCR = 0;
			TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		
		case 0xA0:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		break;
		default:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		break;
	}
}
ISR(ADC_vect){
	switch(MULTIPLEXADO){
		case 1:
		valorADC = ADCH;
		pinADC(0, 1);
		break;
		case 2:
		break;
		default:
		MULTIPLEXADO = 0;
		pinADC(1, 1);
		break;
	}
	MULTIPLEXADO++;
	ADCSRA |= (1 << ADSC);
}