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
	SPCR |= (1<<SPIE);
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
	if(SPIVALOR == 'c'){
		SPDR = POT1;
	}
	else if(SPIVALOR == 'd'){
		SPDR = POT2;
	}
}