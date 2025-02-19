/*
 * Prelab3.asm
 *
 * Creado: 18-Feb-25 22:50 PM 
 * Autor: Rodrigo Estrada
 */
 //Iniciamos nuestro codigo
.include "M328PDEF.inc"
.cseg 
.org 0x0000
	//RJMP start 
.org PCI0addr
	//RJMP RUTINA_INTERRUPCION

//Configuracion de la pila
	LDI R16, LOW(RAMEND)
	OUT SPL, R16
	LDI R16, HIGH(RAMEND)
	OUT SPH, R16

//Configuracion del micro
SETUP:
	//Limpiamos todas las interrupciones globales
	CLI

	//Configuracion de entradas y salidas 
	LDI R16, 0X00
	OUT DDRB, R16 //Establecemos el PORTB como entrada 
	LDI R16, 0XFF
	OUT PORTB, R16 //Activamos los pull-ups de los pines.

	LDI R16, 0XFF
	OUT DDRC, R16 //Establecemos PORTC como una salida 
	LDI R16, 0X00
	OUT PORTC, R16 //Desactivamos todos los leds

	//Configuracion de mi interrupcion 
	LDI R16, (1 << PCINT0) | (1 << PCINT1)
	STS PCMSK0, R16 //Configuramos la mascara para poder realizar las interrupciones en los pines PB0 y PB1 

	LDI R16, (1 << PCIE0)
	STS PCICR, R16 

