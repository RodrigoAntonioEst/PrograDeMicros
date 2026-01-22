/*
 * _7segmentos_anodo.c
 *
 * Created: 18/01/2026 14:21:16
 *  Author: rodro
 */ 
#include <avr/io.h>
#include "7segmentos_anodo.h"
//Establecemos el array de 7 segmentos
const uint8_t segmentos[6] = {
	0b10010010, // 5
	0b10011001, // 4
	0b10110000, // 3
	0b10100100, // 2
	0b11111001, // 1
	0b11000000, // 0
	};

void display_anodo(uint8_t posicion){
	PORTD = segmentos[posicion];
}