/*
 * TIMER0.h
 *
 * Created: 30/03/2026 18:38:49
 *  Author: rodro
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//funcion para inicializar el timer 0
void TIMER0_INIT(uint8_t modo, uint16_t prescaler, uint8_t cuenta);




#endif /* TIMER0_H_ */