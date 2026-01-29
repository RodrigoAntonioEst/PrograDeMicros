/*
 * lab3_spi_digital2.c
 *
 * Created: 28/01/2026 22:16:59
 * Author : rodro
 */ 


/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SPI/SPI.h"
uint8_t valorSPI = 0;
/****************************************/
// Function prototypes
void refreshPORT(uint8_t valor);
void setup(void);
/****************************************/
// Main Function
int main(void)
{
	setup();
	SPIinit(SPI_MASTER_OSC_DIV128, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	/* Replace with your application code */
	while (1)
	{
		PORTC &= ~(1<<PORTC5); //Indicamos que funcione como esclavo
		
		SPIwrite('c');
		
		SPIwrite(0X00);
		
		valorSPI = spiRead();
		refreshPORT(valorSPI);
		
		PORTC |= (1<<PORTC5); //apagamos el slave  
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
	DDRC |= (1<<DDC5);
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	DDRB |= (1<<DDB0)|(1<<DDB1);
	//MSS para seleccionar el slave
	PORTC |= (1<<PORTC5);
	//limpiando el puerto de leds = 0
	PORTB &= ~((1<< PORTB0)|(1<<PORTB1));
	PORTD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7));
}	
/****************************************/
// Interrupt routines
