/*
 * CFile1.c
 *
 * Created: 6/05/2025 12:23:57
 *  Author: rodro
 */ 
#include <avr/io.h>
#include "PWM.h"

void PWM0CONFIG(uint8_t INVERT0, uint8_t MODO0, uint16_t Prescaler0){
	//CONFIGURAMOS OCR0A y OCR0B
	DDRD |= (1<<DDD6);
	DDRD |= (1<<DDD5);
	
	//modo invertido o no 
	switch(INVERT0){
		case 1: 
			//para no invertido 
			TCCR0A |= (1<<COM0A1) | (1<<COM0B1);
		break;
		case 2:
			//para invertido
			TCCR0A |= (1<< COM0A1) | (1<<COM0A0) | (1<<COM0B1) | (1<<COM0B0);
		break;
		default:
			//por defecto no invertido
			TCCR0A |= (1<<COM0A1) | (1<<COM0B1);
		break;
	}
	//modo fast o modo phase correct
	switch(MODO0){
		case 1:
			//modo fast
			TCCR0A |= (1<<WGM01) | (1<<WGM00);
			break;
		case 2:
			//modo phase correct
			TCCR0A |= (1<<WGM00);
		break;
		default:
			//modo fast si se pone un modo no valido 
			TCCR0A |= (1<<WGM01) | (1<<WGM00);
		break;
	}
	//seleccionamos prescaler
	switch(Prescaler0){
		case 1:
			//prescaler de 1
			TCCR0B |= (1<<CS00);
		break;
		case 8:
			//prescaler de 8
			TCCR0B |= (1<<CS01);
		break;
		case 64:
			//prescaler de 64
			TCCR0B |= (1<<CS01) | (1<<CS00);
		break;
		case 256:
			//prescaler de 256
			TCCR0B |= (1<<CS02) | (1<<CS00);
		break;
		case 1024:
			//prescaler de 1024
			TCCR0B |= (1<<CS02) | (1<<CS00);
		break;
		default:
			//prescaler de 64
			TCCR0B |= (1<<CS01) | (1<<CS00);
		break;	
	}
	
	
	
}
void duty0A(uint8_t Val0A_inf, uint8_t Val0A_sup,uint8_t POT0A){
	OCR0A = (((POT0A*(Val0A_sup-Val0A_inf))/255)+Val0A_inf);
}
void duty0B(uint8_t Val0B_inf, uint8_t Val0B_sup, uint8_t POT0B){
	OCR0B = (((POT0B*(Val0B_sup - Val0B_inf))/255)+Val0B_inf);
}
void PWM1CONGIF(uint8_t INVERT1, uint8_t MODO1,uint16_t prescaler1 ,uint16_t top1){
	
	//CONFUGURAMOS OCR1B Y OCR1A
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB1);
	
	//Modo invertido o no
	switch(INVERT1){
		case 1:
			//modo no invertido 
			TCCR1A |= (1<<COM1A1) | (1<<COM1B1);
		break;
		case 2:
			//modo invertido 
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0);
		break;
		default:
			//numero no valido selecciona no invertido 
			TCCR1A |= (1<<COM1A1) | (1<<COM1B1);
		break;
	}
	
	//Fast o phase correct 
	switch(MODO1){
		case 1:
			//fast pwm 
			TCCR1A |= (1<<WGM11);
			TCCR1B |= (1<<WGM12) | (1<<WGM13);
		break;
		case 2:
			//phase correct 
			TCCR1A |= (1<<WGM11);
			TCCR1B |= (1<<WGM13);
		break;
		default:
			//modo no valido 
			//fast pwm
			TCCR1A |= (1<<WGM11);
			TCCR1B |= (1<<WGM12) | (1<<WGM13);
		break;
	}
	
	//seleccionamos prescaler
	switch(prescaler1){
		case 1: 
			//prescaler de 1 
			TCCR1B |= (1<<CS10);
		break;
		case 8:
			//prescaler de 8
			TCCR1B |= (1<<CS11);
		break;
		case 64:
			//prescaler de 64
			TCCR1B |= (1<<CS11) | (1<<CS10);
		break;
		case 256:
			//prescaler de 256
			TCCR1B |= (1<<CS12);
		break;
		case 1024:
			//prescaler de 1024
			TCCR1B |= (1<<CS10) | (1<<CS12);
		break;
		default:
			//numero no valido prescaler de 64
			TCCR1B |= (1<<CS11) | (1<<CS10);
		break;
	}
	
	//colocamos el top
	ICR1 = top1;
}

void duty1A(uint16_t Val1A_inf, uint16_t val1A_sup, uint8_t POT1A){
	OCR1A = ((POT1A*(val1A_sup-Val1A_inf))/255)+Val1A_inf;
}
void duty1B(uint16_t Val1B_inf, uint16_t Val1B_sup, uint8_t POT1B){
	OCR1B = ((POT1B*(Val1B_sup-Val1B_inf))/255)+Val1B_inf;
}
void PWM2CONFIG(uint8_t INVERT2,uint8_t MODO2,uint16_t prescaler2){
	//habilitamos OCR2B
	DDRD |= (1<<DDD3);
	
	//Invertido o no 
	switch(INVERT2){
		case 1:
			//elejimos no invertido
			TCCR2A |= (1<<COM2B1);
		break;
		case 2:
			//elejimos invertido
			TCCR2A |= (1<<COM2B1) | (1<<COM2B0);
		break;
		default:
			//elejimos no invertido
			TCCR2A |= (1<<COM2B1);
		break;
	}
	
	//elegimos FAST o PHASE-CORRECT
	switch(MODO2){
		case 1:
			//Elegimos fast
			TCCR2A |= (1<<WGM20) | (1<<WGM21);
		break;
		case 2:
			//elegimos phase correct 
			TCCR2A |= (1<<WGM20);
		break;
		default:
			//Elegimos fast
			TCCR2A |= (1<<WGM20) | (1<<WGM21);
		break;
	}
	//escogemos prescaler 
	switch(prescaler2){
		case 1:
			//prescaler de 1 
			TCCR2B |= (1<<CS20);
		break;
		case 8:
			//prescaler de 8
			TCCR2B |= (1<<CS21);
		break;
		case 32:
			//prescaler de 32
			TCCR2B |= (1<<CS21) | (1<<CS20);
		break;
		case 64:
			//prescaler de 64
			TCCR2B |= (1<<CS22);
		break;
		case 128:
			//prescaler de 128
			TCCR2B |= (1<<CS20) | (1<<CS22);
		break;
		case 256:
			//prescaler de 256
			TCCR2B |= (1<<CS21) | (1<<CS22);
		break;
		case 1024:
			//prescaler de 1024
			TCCR2B |= (1<<CS20) | (1<<CS21) | (1<<CS22);
		break;
		default:
			//prescaler de 64
			TCCR2B |= (1<<CS22);
		break;
	}
}
void duty2A(uint8_t val2A_inf, uint8_t val2A_sup, uint8_t POT2A){
	OCR2B = (((POT2A*(val2A_sup - val2A_inf))/255)+val2A_inf);
}
//Funcion de escritura en eeprom
void write(uint16_t dirrecion, uint8_t dato){
	while (EECR & (1 << EEPE));  // Esperar si hay una escritura en curso
	EEAR = dirrecion;            // Dirección de EEPROM
	EEDR = dato;                 // Dato a escribir
	EECR |= (1 << EEMPE);        // Habilitar escritura
	EECR |= (1 << EEPE);         // Iniciar escritura
}
//funcion de lectura en eeprom 
uint8_t read(uint16_t dirrecion){
	while (EECR & (1 << EEPE));  // Esperar si hay una escritura en curso
	EEAR = dirrecion;              // Dirección de EEPROM
	EECR |= (1 << EERE);         // Iniciar lectura
	return EEDR;
}