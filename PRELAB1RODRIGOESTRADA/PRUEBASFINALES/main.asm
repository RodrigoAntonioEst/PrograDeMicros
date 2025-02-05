/*
 * Prelab1.asm
 *
 * Creado: 4-Feb-25 4:02 PM
 * Autor: Rodrigo Estrada
 */

 .include "M328PDEF.inc"
 
 .cseg
 .org 0x0000

 // Configuración de la pila
 LDI R16, LOW(RAMEND)
 OUT SPL, R16
 LDI R16, HIGH(RAMEND)
 OUT SPH, R16

 // Configuración de MCU
 SETUP:
    LDI R16, 0x00 
    OUT DDRB, R16  // PORTB como entrada
    LDI R16, 0xFF
    OUT PORTB, R16 // Habilitar pull-ups en PORTB

    LDI R16, 0xFF
    OUT DDRC, R16 //PORTC como salida
    LDI R16, 0x00
    OUT PORTC, R16 //Desactivamos los leds 

    LDI R17, 0xFF  //Estado inicial de botones (todos en pull-up)
    LDI R19, 0x00  //Registro de conteo

 // Loop principal
LOOP:
    IN R16, PINB  //hacemos una lectura en los botones
    CP R16, R17  //Usamos esta funcion para comparar si lo que esta en R17 y R16 es lo mismo
    BREQ LOOP //si es lo mismo se regresa al loop inicial
DELAY: //usamos la funcion delay para poder evitar rebotes en los push 
	LDI R18, 0
SUBDELAY1: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa 
	INC R18
	CPI R18, 0
	BRNE SUBDELAY1
	LDI R18, 0
SUBDELAY2://el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R18
	CPI R18, 0
	BRNE SUBDELAY2
	LDI R18, 0
SUBDELAY3: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R18
	CPI R18, 0
	BRNE SUBDELAY3
SUBDELAY4: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R18
	CPI R18, 0
	BRNE SUBDELAY4 //aqui se terminan los delay para evitar lo rebotes
SUBDELAY5: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R18
	CPI R18, 0
	BRNE SUBDELAY5 //aqui se terminan los delay para evitar lo rebotes

    IN R16, PINB  //se leen de nuevo los botones, luego de cierta cantidad de tiempo 
    CP R16, R17  //si al hacer la comparacion se detecta que el push sigue en el mismo estado se manda de regreso al loop
    BREQ LOOP	//si es igual manda de regreso al loop
	MOV R17, R16 //se guarda el nuevo estado
	SBRS R17, 0 //si el bit 0 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    INC R19 //se realiza el incremento 
	SBRS R17, 1   ////si el bit 1 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    DEC R19 //se realiza el decremento 
	OUT PORTC, R19 //se mandan los datos 
    RJMP LOOP //se regresa al bucle infinito
