/*
 * Lab6RozottoDigital2.c
 *
 * Created: 12/03/2026 20:33:50
 * Author : jp_rr
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

//---------------- UART ----------------//

void UART_init(void)
{
	uint16_t ubrr = 103; //9600 baud para 16MHz

	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;

	UCSR0B = (1<<TXEN0);                 // habilitar TX
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);  // 8 bits
}

void UART_sendChar(char data)
{
	while (!(UCSR0A & (1<<UDRE0))); // esperar buffer libre
	UDR0 = data;
}

void UART_sendString(char *str)
{
	while(*str)
	{
		UART_sendChar(*str++);
	}
}

//--------------------------------------//

int main(void)
{

	UART_init();

	// PC0-PC5 como entrada
	DDRC &= ~(0b00111111);

	// activar pull-ups
	PORTC |= 0b00111111;

	uint8_t botones;

	while(1)
	{

		// leer puerto
		botones = PINC & 0b00111111;

		if(!(botones & (1<<PC0)))
		UART_sendString("Boton 0 presionado\r\n");

		if(!(botones & (1<<PC1)))
		UART_sendString("Boton 1 presionado\r\n");

		if(!(botones & (1<<PC2)))
		UART_sendString("Boton 2 presionado\r\n");

		if(!(botones & (1<<PC3)))
		UART_sendString("Boton 3 presionado\r\n");

		if(!(botones & (1<<PC4)))
		UART_sendString("Boton 4 presionado\r\n");

		if(!(botones & (1<<PC5)))
		UART_sendString("Boton 5 presionado\r\n");

		_delay_ms(200);
	}
}