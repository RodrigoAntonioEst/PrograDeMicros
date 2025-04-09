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
		uint16_t valor = (((ADCH*(30))/255)+7);
		OCR1A = valor;
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
	DDRB |= (1 << DDB1);
	
	//Configuramos la frecuencia de micro a 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//configuramos nuestro PWM en modo FAST
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1<< CS11 ) | (1 << CS10);
	ICR1 = 312;

	
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	ADCSRA |= (1 << ADSC);
	
} 