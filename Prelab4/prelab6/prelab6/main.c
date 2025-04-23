/*
 * prelab6.c
 *
 * Created: 22/04/2025 22:47:38
 * Author : rodrigo
 */ 
//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
//
// Function prototypes
void setup(void);
void UART_init(void);
void writechar(char caracter);
//
// Main Function
int main(void)
{
	setup();
	UART_init();
	writechar('P');
	while (1)
	{
		
	}
}
//
// NON-Interrupt subroutines
void setup(void){
cli();
UART_init();
sei();
}
void UART_init(void)
{
	//Establecer TX como salida y RX como entrada  
	DDRD |= (1<<DDD1);
	DDRD &= ~(1<<DDD0);
	//configurar UCSR0A
	UCSR0A = 0;
	//Configurar UCSR0B
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	//Configurar UCSR0C, Configurar asincrono, sin polaridad. 1 stop bit y 8 bit de datos
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	//Configuramos baudrate 9600 a 16MHz
	UBRR0 = 103;
}
void writechar(char caracter)
{
 UDR0 = caracter;
}
//
// Interrupt routines 