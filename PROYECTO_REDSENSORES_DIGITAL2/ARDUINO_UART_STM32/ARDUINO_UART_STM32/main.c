/*
 * ARDUINO_UART_STM32.c
 *
 * Created: 12/03/2026 19:43:53
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdint.h"
#include "UART/UART_RECEIVER.h"
/****************************************/
// Function prototypes
void setup(void);
/****************************************/
// Main Function
int main(void)
{
	UART_RECEIVER(103);
	setup();
	/* Replace with your application code */
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();
		DDRC &= ~((1<<DDC0)|(1<<DDC1)|(1<<DDC2)|(1<<DDC3)|(1<<DDC4)|(1<<DDC5)); // entradas
		PORTC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2)|(1<<PORTC3)|(1<<PORTC4)|(1<<PORTC5); // pull-up
		
		PCICR |= (1<<PCIE1);
		PCMSK1 |= (1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11)|(1<<PCINT12)|(1<<PCINT13);
	sei();
	}
/****************************************/
// Interrupt routines
ISR(PCINT1_vect){
	if (!(PINC & (1<<PINC0))){
		cadena("A");
	}
	else if (!(PINC & (1<<PINC1))){
		cadena("B");
	}
	else if (!(PINC & (1<<PINC2))){
		cadena("C");
	}
	else if (!(PINC & (1<<PINC3))){
		cadena("D");
	}
	else if (!(PINC & (1<<PINC4))){
		cadena("E");
	}
	else if (!(PINC & (1<<PINC5))){
		cadena("F");
	}
}
