/*
 * UART_RECEIVER.c
 *
 * Created: 26/01/2026 08:11:21
 *  Author: rodro
 */ 
#include "UART_RECEIVER.h"
void UART_RECEIVER(uint8_t baudrate){
	//Ponemos como salida tx y entrada rx
	DDRD |= (1<<DDD1); //TX
	DDRD &= ~(1<<DDD0); //RX
	
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = baudrate; // baudrate 9600 a 16MHz
	
	
}
//Rutina para recibir un caracter
void writechar(char caracter)
{
	while((UCSR0A & (1 << UDRE0))== 0);
	UDR0 = caracter;
}
//rutina para recibir una cadena
void cadena(char* frase){
	for(uint8_t i = 0; *(frase+i) != '\0'; i++){
		writechar(*(frase+i));
	}
}