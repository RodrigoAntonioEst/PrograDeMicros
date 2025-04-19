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
		//MULTIPLEXACION
		if (MULTIPLEXACION == 0){
			//Configuramos el ADC
			ADMUX = 0;
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2);
			POT1 = ADCH;
			CICLODETRABAJO(POT1,7,37);
		}
		else if(MULTIPLEXACION == 1){
			//Configuramos el ADC
			ADMUX = 0;
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX2);
			POT2 = ADCH;
			CICLODETRABJO0(POT2,7,37);
		}
		else{
			ADMUX =0; 
			ADMUX |= (1 << REFS0) | (1 << ADLAR ) | ( 1<< MUX0 );
			PWMCOMP = ADCH;
			if(TCNT2 >= PWMCOMP){
				PORTD &= ~(1 << PORTD5);
			}
		}
		
	}
}
//
// NON-Interrupt subroutines
void setup(){
	cli();
	
	DDRD |= (1 << DDD5);
	PORTD |= (1 << PORTD5);
	
	TCCR0B |= (1 << CS01) | (1 << CS00);
	TCNT0 = 177;
	TIMSK0 |= (1 << TOIE0);
	
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
	ADCSRA |= (1 << ADSC);
} 
ISR(TIMER0_OVF_vect){
	MULTIPLEXACION++;
	TCNT0 = 177;
	if(MULTIPLEXACION >= 3){
		MULTIPLEXACION = 0;
	};  
}
ISR(TIMER2_OVF_vect){
	TCNT2 = 0;
	PORTD |= (1 << PORTD5);
}