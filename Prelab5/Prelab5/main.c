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
#include "PWM/PWM.h"
//
// Function prototypes
void setup();
void PMW1CONFIG(uint16_t top, uint16_t prescaler);
void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP);

//
// Main Function
int main(void)
{
	setup();
	PMW1CONFIG(312,64);
	while (1)
	{
		CICLODETRABAJO(ADCH,7,37);
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
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	ADCSRA |= (1 << ADSC);
	
} 