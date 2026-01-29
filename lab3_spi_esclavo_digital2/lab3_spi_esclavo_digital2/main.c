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
volatile uint8_t VALORADC = 0;
uint8_t contador = 0;
/****************************************/
// Function prototypes
void refreshPORT(uint8_t valor);
/****************************************/
// Main Function
int main(void)
{
	cli();
	SPIinit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	init_ADC(0, 128, 0);
	SPSR |= (1<<SPIE);
	sei();
	while (1)
	{
		
	}
}
/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines
ISR(ADC_vect){
	VALORADC = ADCH;
	ADCSRA |= (1<<ADSC);
}
ISR(SPI_STC_vect){
	uint8_t SPIVALOR = SPDR;
	if(SPDR == 'c'){
		SPDR = VALORADC;
	}
}