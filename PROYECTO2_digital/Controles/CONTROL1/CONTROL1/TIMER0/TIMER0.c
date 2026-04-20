/*
 * TIMER0.c
 *
 * Created: 30/03/2026 18:39:28
 *  Author: rodro
 */ 
#include "TIMER0.h"

//libreria de inicializacion de timer0
void TIMER0_INIT(uint8_t modo, uint16_t prescaler, uint8_t cuenta){
	//----------------------- Seleccion de modo ----------------//
	switch(modo){
		case 0:
			TCCR0A = 0;
		break;
		case 1:
			TCCR0A = (1<<WGM01);
		break;
		default:
			TCCR0A = 0;
		break;
	}
	
	//--------------------- Seleccion de prescalers ------------//
	switch(prescaler){
		case 1:
			TCCR0B |= (1<<CS00);
		break;
		case 8:
			TCCR0B |= (1<<CS01);
		break;
		case 64:
			TCCR0B |= (1<<CS01)|(1<<CS00);
		break;
		case 256:
			TCCR0B |= (1<<CS02);
		break;
		case 1024:
			TCCR0B |= (1<<CS00)|(1<<CS02);
		break;
		default:
			//NADA
		break;
	}
	//-------------------- CARGA DE TCNT0 -------------------//
	TCNT0 = cuenta;
	//-------------------- Habilatamos interrupcion ---------//
	TIMSK0 = (1<<TOIE0);
}
