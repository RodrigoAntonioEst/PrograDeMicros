/*
 * PWM.c
 *
 * Created: 8/04/2025 23:48:25
 *  Author: rodro
 */ 
#include <avr/io.h>
#include "PWM.h"

void PMW1CONFIG(uint16_t top, uint16_t prescaler) {
	//CONFIGURAMOS 0CR1A COMO LA SALIDA EN PB1
	DDRB |= (1 << DDB1);
	
	//CONFIGURAMOS FASTPWM
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13);
	
	//EL PROGRAMADOR INGRESARA EL TOP 
	ICR1 = top;
	
	//EL PROGRAMADOR INGRESARA EL PRESCALER
	switch (prescaler) {
		case 1:
		TCCR1B |= (1 << CS10);
		break;
		case 8:
		TCCR1B |= (1 << CS11);
		break;
		case 64:
		TCCR1B |= (1 << CS11) | (1 << CS10);
		break;
		case 256:
		TCCR1B |= (1 << CS12);
		break;
		case 1024:
		TCCR1B |= (1 << CS12) | (1 << CS10);
		break;
		default:
		// Si el valor no es v�lido, usar prescaler = 64 por defecto
		TCCR1B |= (1 << CS11) | (1 << CS10);
		break;
	}
}

void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP){
	uint16_t valor = (((VAL*(LIMITE_SUP - LIMITE_INF))/255)+LIMITE_INF);
	OCR1A = valor;
}
void PWM2CONFIG(uint16_t prescaler2){
	
	DDRB |= (1 << DDB3);
	
	//seleccionamos no invertido
	TCCR2A |= (1 << COM2A1);
	
	//iniciamos el modo fast pwm 
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	
	//seleccionamos el prescaler 
	switch (prescaler2) {
		case 1:
		TCCR2B |= (1 << CS20);
		break;
		case 8:
		TCCR2B |= (1 << CS21);
		break;
		case 64:
		TCCR2B |= (1 << CS22);
		break;
		case 256:
		TCCR2B |= (1 << CS22);
		break;
		case 1024:
		TCCR2B |= (1 << CS22) | (1 << CS20);
		break;
		default:
		// Si el valor no es v�lido, usar prescaler = 64 por defecto
		TCCR2B |= (1 << CS21) | (1 << CS20);
		break;
	}
	
}
void CICLODETRABJO0(uint16_t VAL0 ,uint16_t LIMITE_INF0, uint16_t LIMITE_SUP0){
	OCR2A = (((VAL0*(LIMITE_SUP0 - LIMITE_INF0))/255)+LIMITE_INF0);
	
}
