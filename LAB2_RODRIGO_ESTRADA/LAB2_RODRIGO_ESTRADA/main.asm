/*
 * LAB2.asm
 *
 * Creado: 12-Feb-25 4:04 PM
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

//Configuracion de microprocesador
SETUP:

//configuramos la salida
LDI R16, 0XFF
OUT DDRD, R16 //Configuramos la el PORTD como una salida 
LDI R16, 0X00
OUT PORTD, R16 //Desactivamos todos los leds 

//configuramos las entradas para los pushbuttons
LDI R16, 0X00
OUT DDRB, R16 //Configuramos el PORTB como una entrada 
LDI R16, 0XFF
OUT PORTB, R16 //Habilitamos todos los pull-ups



LDI R20, 0b11000000 //0
LDI ZL, 0X00
LDI ZH, 0X01
ST Z+, R20

LDI R20, 0b11111001 //1
ST Z+, R20

LDI R20, 0b10100100 //2
ST Z+, R20

LDI R20, 0b00110000 //3
ST Z+, R20 

LDI R20, 0b00011001 //4
ST Z+, R20 

LDI R20, 0b00010010 //5
ST Z+, R20 

LDI R20, 0b00000010 //6
ST Z+, R20

LDI R20, 0b01111000 //7
ST Z+, R20 

LDI R20, 0b00000000 //8
ST Z+, R20

LDI R20, 0b00010000//9
ST Z+, R20 

LDI R20, 0b00001000 //A
ST Z+, R20 

LDI R20, 0b00000011 //B
ST Z+, R20

LDI R20, 0b01000110 //C
ST Z+, R20 

LDI R20, 0b00100001 //D
ST Z+, R20 

LDI R20, 0b00000110 //E
ST Z+, R20 

LDI R20, 0b00001110 //F
ST Z+, R20 

LDI ZL, 0X00
LDI ZH, 0X01 
LD R20, Z 
OUT PORTD, R20


LDI R17, 0XFF 
LDI R21, 0X00

//LOOP principal
LOOP:
	CALL ANTIREBOTE
	CALL CONTADOR_DE_4_BITS
	RJMP LOOP

//Configuramos el antirebote
ANTIREBOTE: 
	IN R16, PINB  //hacemos una lectura en los botones
	CP R16, R17  //Usamos esta funcion para comparar si lo que esta en R17 y R16 es lo mismo
	BREQ ANTIREBOTE //si es lo mismo se regresa al loop inicial
	CALL DELAY
	IN R16, PINB  //se leen de nuevo los botones, luego de cierta cantidad de tiempo 
    CP R16, R17  //si al hacer la comparacion se detecta que el push sigue en el mismo estado se manda de regreso al loop
    BREQ ANTIREBOTE	//si es igual manda de regreso al loop
	MOV R17, R16 //se guarda el nuevo estado
	RET 

//Contador de 4 bits 
CONTADOR_DE_4_BITS:
 
	
//subrutina de interrupcion 
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
	RET