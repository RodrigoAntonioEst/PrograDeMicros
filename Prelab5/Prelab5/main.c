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
void PMW1CONFIG(uint16_t top, uint16_t prescaler); //ponemos el prototipo de la configuracion PWM1
void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP); //este es el protipo para OCR1A
void CICLODETRABJO0(uint16_t VAL0, uint16_t LIMITE_INF0, uint16_t LIMITE_SUP0); //este es el protipo para OCR1B

//
// Main Function
int main(void)
{
	setup();//llamamos la funcion de setup
	PMW1CONFIG(312,64); //configuramos el PWM de de OCR1A y OCR1B
	while (1)
	{
	
						
	}
}
//
// NON-Interrupt subroutines
void setup(){
	cli();
	
	DDRD |= (1 << DDD5); //configuramos el puerto del PWM manual
	PORTD |= (1 << PORTD5); //ponemos un 1 logico
	
	TCCR2B |= (1 << CS22); //configuramos el prescaler del TIMER2
	TCNT2 = 0; //le cargamos el valor 0 para que cuente hasta 255 teniendo un periodo de 16ms
	TIMSK2 |= (1 << TOIE2); //activamos la interrupcion de TIMER2 por overflow 
	
	ADCSRA = 0; //eliminamos las configuraciones previos del ADC 
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC); //activamos prescaler, interrupcion e iniciamos la conversion 
	
	//Configuramos la frecuencia de micro a 1MHz
	CLKPR = (1 << CLKPCE); 
	CLKPR = (1 << CLKPS2);	
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	//hacemos un switch case para que cada vez que se termine de realizar una conversion este aumente un contador y compara de que pin de ADC viene la senal
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
	MULTIPLEXACION++; //incremento cada vez que se termina de convertir el ADC 
	ADCSRA |= (1 << ADSC); //iniciamos una nueva conversion
} 
ISR(TIMER2_OVF_vect){
	TCNT2 = 0;
	if(PWMCOMP == 0){
		PORTD &= ~(1 << PORTD5); //si ya llego a 0 el potenciometro que apague el por completo la luz para evitar errores 
	}
	else {
		PORTD |= (1 << PORTD5); //de la contrario enciende la led hasta que compare 
	}
	
}