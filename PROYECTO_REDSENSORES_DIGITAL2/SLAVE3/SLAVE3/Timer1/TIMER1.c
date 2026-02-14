/*
 * TIMER1.c
 *
 * Created: 13/02/2026 17:56:30
 *  Author: rodro
 */
#include "TIMER1.h" 
void timer1_init(uint16_t prescaler, uint16_t tiempo){
	cli();
	TIFR1 = (1<<TOV1);
	TCCR1A = 0;
	TCCR1B = 0;
	switch(prescaler){
		case 8:
		TCCR1B |= (1<<CS11);
		break;
		case 64:
		TCCR1B |= (1<<CS11)|(1<<CS10);
		break;
		case 256:
		TCCR1B |= (1<<CS12);
		break;
		case 1024:
		TCCR1B |= (1<<CS12) | (1<<CS10);
		break;
		default:
		TCCR1B |= (1<<CS10);
		break;
	}
	TCNT1 = tiempo;
	TIMSK1 |= (1<<TOIE1);
}