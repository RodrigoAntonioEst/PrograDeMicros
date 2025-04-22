/*
 * Prelab5.c
 *
 * Created: 8/04/2025
 * Author: Rodrigo Estrada
 * Description: Mover servo con PWM
 */
//
// Encabezado (Libraries)
#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM/PWM.h"
uint8_t MULTIPLEXACION;
uint8_t POT1; 
uint8_t POT2;
uint8_t PWMCOMP;

//
// Function prototypes
void setup();
void PMW1CONFIG(uint16_t top, uint16_t prescaler);
void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP);
void CICLODETRABJO0(uint16_t VAL0, uint16_t LIMITE_INF0, uint16_t LIMITE_SUP0);

//
// Main Function
int main(void)
{
	setup();
	PMW1CONFIG(312,64);
	//PWM2CONFIG(64);
	while (1)
	{
	
						
	}
}
//
// NON-Interrupt subroutines
void setup(){
	cli();
	
	DDRD |= (1 << DDD5);
	PORTD |= (1 << PORTD5);
	
	TCCR2B |= (1 << CS22);
	TCNT2 = 0;
	TIMSK2 |= (1 << TOIE2);
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC);
	
	//Configuramos la frecuencia de micro a 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);	
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	switch(MULTIPLEXACION){
		case 1:
		PWMCOMP = ADCH;
		if(TCNT2 >= PWMCOMP){
			PORTD &= ~(1 << PORTD5);
		}
		ADMUX = 0;
		ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2);
		break;
		case 2:
		POT1 = ADCH;
		CICLODETRABAJO(POT1,7,37);
		ADMUX = 0;
		ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX2);
		break;
		case 3:
		POT2 = ADCH;
		CICLODETRABJO0(POT2,7,37);
		break;
		default:
		ADMUX =0;
		ADMUX |= (1 << REFS0) | (1 << ADLAR ) | ( 1<< MUX0 );
		MULTIPLEXACION = 0;
		break;
	}
	MULTIPLEXACION++;
	ADCSRA |= (1 << ADSC);
} 
ISR(TIMER2_OVF_vect){
	TCNT2 = 0;
	if(PWMCOMP == 0){
		PORTD &= ~(1 << PORTD5);
	}
	else {
		PORTD |= (1 << PORTD5);
	}
	
}