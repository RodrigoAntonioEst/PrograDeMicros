  /*
 * lab3_spi_esclavo_digital2.c
 *
 * Created: 29/01/2026 00:46:47
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SPI/SPI.h"
#include "ADC/ADC.h"
volatile uint8_t VALORADC;
volatile uint8_t contador;
volatile uint8_t POT1;
volatile uint8_t POT2;
volatile uint8_t MULTIPLEXADO;
volatile uint8_t flag = 0;
volatile uint8_t contador;
volatile uint8_t led_val = 0;

/****************************************/
// Function prototypes
void refreshPORT(uint8_t valor);
/****************************************/
// Main Function
int main(void)
{
	cli();
	SPIinit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	init_ADC(1, 128, 0);
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	DDRB |= (1<<DDB0)|(1<<DDB1);
	
	//limpiando el puerto de leds = 0
	PORTB &= ~((1<< PORTB0)|(1<<PORTB1));
	PORTD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7));
	SPCR |= (1<<SPIE);
	sei();
	while (1)
	{
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
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	switch(MULTIPLEXADO){
		case 1:
		POT1 = ADCH;
		pinADC(0, 1);
		break;
		case 2:
		POT2 = ADCH;
		break;
		default:
		MULTIPLEXADO = 0;
		pinADC(1, 1);
		break;
	}
	MULTIPLEXADO++;
	ADCSRA |= (1 << ADSC);
}

ISR(SPI_STC_vect){
    uint8_t SPIVALOR = SPDR;

    if(flag == 0){
        if(SPIVALOR == 'c'){
            SPDR = POT1;          // respuesta al siguiente byte
        }
        else if(SPIVALOR == 'd'){
            SPDR = POT2;
        }
        else if(SPIVALOR == 'L'){
            flag = 1;        // el próximo byte será el valor LED
            SPDR = 0x00;          // no importa qué mandemos aquí
        }
        else{
            SPDR = 0x00;
        }
    }
    else if(flag == 1){
        led_val = SPIVALOR;
        refreshPORT(led_val);     // mostrar en LEDs del esclavo
        flag = 0;
        SPDR = 0x00;
    }
}
