/*
 * UART_RECEIVER.h
 *
 * Created: 26/01/2026 08:11:07
 *  Author: rodro
 */ 


#ifndef UART_H_
#define UART_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
//funcion para poder comunicarme con el nano
void UART_INIT(uint8_t baudrate);
void writechar(char caracter);
void cadena(char* frase);


#endif /* UART_H_ */