/*
 * Prelab5.c
 *
 * Created: 8/04/2025
 * Author: Rodrigo Estrada
 * Description: Mover servo con PWM
 */
//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
volatile uint8_t pot;
//
// Function prototypes
void setup();
//
// Main Function
int main(void)
{
	setup();
	while (1)
	{
		uint8_t valor = (((ADCH*30)/255)+7);
		OCR0A = valor;
	}
}
//
// NON-Interrupt subroutines
void setup(){
	cli();
	
	//Configuramos el ADC 
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2); 

	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC);
	
	//configuramos el pin D6 para sacar el pwm
	DDRD |= (1 << DDD6);
	
	//Configuramos la frecuencia de micro a 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//configuramos nuestro PWM en modo FAST
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	//Configuramos el periodo a 16ms
	TCCR0B |= (1 << CS01) | (1 << CS00);
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	ADCSRA |= (1 << ADSC);
	
}



