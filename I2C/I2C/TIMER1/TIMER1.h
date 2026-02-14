/*
 * TIMER1.h
 *
 * Created: 13/02/2026 17:56:13
 *  Author: rodro
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
//Funcion para inicializar el timer0
void timer1_init(uint16_t prescaler, uint16_t tiempo);




#endif /* TIMER1_H_ */