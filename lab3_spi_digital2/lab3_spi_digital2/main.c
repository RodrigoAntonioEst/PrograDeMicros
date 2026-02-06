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
#include <stdlib.h>
#include "SPI/SPI.h"
#include <util/delay.h>
#include "UART/UART_RECEIVER.h"
volatile uint8_t valorSPI;
char buf[10];
char dato;
char concatenacion[10];
volatile uint8_t num;
volatile uint8_t posicion = 0;
volatile uint8_t conversion;

/****************************************/
// Function prototypes
void refreshPORT(uint8_t valor);
void setup(void);
/****************************************/
// Main Function
int main(void)
{
	setup();
	UART_RECEIVER(103);
	SPIinit(SPI_MASTER_OSC_DIV128, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	sei();
	while (1)
	{
		PORTC &= ~(1<<PORTC5);          // CS LOW

		SPItransfer('c');               // comando
		valorSPI = SPItransfer(0x00);   // dummy para leer respuesta (POT1)

		utoa(valorSPI, buf, 10);
		cadena("Valor POT 1: ");
		cadena(buf);
		cadena("\r\n");

		PORTC |= (1<<PORTC5);           // CS HIGH

		//
		PORTC &= ~(1<<PORTC5);          // CS LOW

		SPItransfer('d');               // comando
		valorSPI = SPItransfer(0x00);   // dummy para leer respuesta (POT2)

		utoa(valorSPI, buf, 10);
		cadena("Valor POT 2: ");
		cadena(buf);
		cadena("\r\n");

		PORTC |= (1<<PORTC5);           // CS HIGH
        
        _delay_ms(50);


		
		  
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
	concatenacion[0] = '\0';
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
ISR(USART_RX_vect){
	dato = UDR0;
	if(dato == '\n'){             
		conversion = atoi(&concatenacion[0]);
		cadena("Numero recibido\r\n");
		refreshPORT(conversion);
        // Mandar el mismo valor al esclavo para que lo muestre en sus LEDs
        PORTC &= ~(1<<PORTC5);      // CS LOW
        SPItransfer('L');           // comando: "LED"
        SPItransfer(conversion);    // dato
        PORTC |= (1<<PORTC5);       // CS HIGH

		posicion = 0;
		concatenacion[0] = '\0';
	}
	else if(posicion < 9){
		concatenacion[posicion++] = dato;
		concatenacion[posicion] = '\0';
	}
}