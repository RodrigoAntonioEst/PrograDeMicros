/*
 * Prelab2.asm
 *
 * Creado: 11-Feb-25 11:10 AM
 * Autor: Rodrigo Estrada
 */

 .include "M328PDEF.inc"

 .cseg
 .org 0x0000

 //configuracion de la pila
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R16, HIGH(RAMEND)
OUT SPH, R16
//Configuracion del Micro
SETUP:
	//Le bajamos nuestra frecuencia a nuestro procesador
	LDI R16, (1 << CLKPCE)
	STS CLKPR, R16
	LDI R16, 0x08 //Le baje la frecuencia a 62.5KHz para poder operar de mejor manera el timer 0 
	STS CLKPR, R16

	//inicializamos el timer0
	LDI R16, (1 << CS01	| 1 << CS00 ) 
	OUT TCCR0B, R16 // Seteamos el prescaler a 1024 del timer0
	LDI R16, 0x9E
	OUT TCNT0, R16 //Cargo el valor inicial del prescaler

	//Configuramos el PORTC como salida
	LDI R16, 0XFF
	OUT DDRC, R16 //establecimos los puertos como salida.
	LDI R16, 0X00
	OUT PORTC, R16

	LDI R18, 0x00 //Aqui guardamos nuestro conteo de de bits para los leds 

//Programa principal
LOOP:
	IN R16, TIFR0 //Leemos el registro TIFR0 para ver si si se llego al valor deseada para interrumpir el timer0
	SBRS R16, TOV0 //esta linea verifica si la bandera de overflow esta activa, y hasta que esta se activa salta.
	RJMP LOOP
	SBI TIFR0, TOV0 //limpiamos la bandera de overflow, le cargamos un 1 para descativarla
	LDI R16, 0x9E
	OUT TCNT0, R16 //Recargamos el valor al timer0 cada vez que haga overflow 
	INC R18 //Incrementamos cada vez que se llega a los 100ms 
	CPI R18, 0X10
	BRBS 1, OVERFLOW
	OUT PORTC, R18 //sacamos a PORTC lo que tengo guardado en r18
	RJMP LOOP
OVERFLOW:
	LDI R18, 0X00
	OUT PORTC, R18
	RJMP LOOP
