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
	LDI R16, (1 << CLKPCE)  ; Cargar el bit CLKPCE en R16
	STS CLKPR, R16          ; Habilitar el cambio del prescaler
	LDI R16, 0b00000100     ; Cargar el valor del prescaler (división por 16)
	STS CLKPR, R16          ; Escribir el nuevo valor en el registro CLKPR

	LDI R16, 0XFF 
	OUT DDRD, R16 //Establecemos el PORTD como salida
	LDI R16, 0X00
	OUT PORTD, R16 //Apagamos todos los leds

    LDI R16, 0x00 
    OUT DDRB, R16  // PORTB como entrada menos PB5
    LDI R16, 0xFF
    OUT PORTB, R16 // Habilitar pull-ups en PORTB, desactivo el led en pb5

    LDI R16, 0xFF
    OUT DDRC, R16 //PORTC como salida
    LDI R16, 0x00
    OUT PORTC, R16 //Desactivamos los leds 

    LDI R17, 0xFF  //Estado inicial de botones (todos en pull-up)
    LDI R19, 0x00  //Registro de conteo1
	LDI R20, 0X00 //Registro de conteo2
	LDI R21, 0X00
	LDI R22, 0X00
	LDI R24, 0X00

 // Loop principal
LOOP:
	CALL Antirebote
    CALL ContadorRojo
	CALL ContadorAzul
    CALL Suma
	RJMP LOOP


//subrutina de suma con carry 
Suma:
    SBRC R17, 4     //Si el bit 4 de R17 es 0, sigue. Si es 1, salta a RETORNO
    RJMP RETORNO
	MOV R21, R19    //Copiar R19 en R21
    SWAP R21        //Intercambiar nibbles de R21
	MOV R22, R20    //Copiar R20 en R22
	MOV R23, R20
    SWAP R22        //Intercambiar nibbles de R22
	ANDI R22, 0XF0
	ADD R21, R22    //Sumar los valores intercambiados
	BRBS 0, salto   //Si NO hay carry, salta a 'salto'
	ANDI R20, 0X0F
    ADD R20, R21    //Si hubo carry, sumar R21 a R16
	OUT PORTD, R20
	RET
salto:
    SBI PINC, 4     //Toggle en PB5
	ADD R23, R24	//Si hubo carry, sumar R21 a R16
	OUT PORTD, R23
RETORNO:
    RET             


//SUB RUTINA DE ANTIREBOTE
Antirebote:
	IN R16, PINB  //hacemos una lectura en los botones
    CP R16, R17  //Usamos esta funcion para comparar si lo que esta en R17 y R16 es lo mismo
    BREQ Antirebote //si es lo mismo se regresa al loop inicial
	CALL DELAY
	IN R16, PINB  //se leen de nuevo los botones, luego de cierta cantidad de tiempo 
    CP R16, R17  //si al hacer la comparacion se detecta que el push sigue en el mismo estado se manda de regreso al loop
    BREQ Antirebote	//si es igual manda de regreso al loop
	MOV R17, R16 //se guarda el nuevo estado
	RET 

//Establecemos la subrutina para el contador 2
ContadorRojo:
	SBRS R17, 2 //si el bit 2 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    INC R20 //se realiza el incremento
	BRBS 1,	ULTIMORECURSO  
	CPI R20, 0X10
	BRNE OVERFLOWROJO
	LDI R20, 0X00
	ADD R20, R22
OVERFLOWROJO:
	SBRS R17, 3   //si el bit 3 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    DEC R20 //se realiza el decremento
	CPI R20, 0XEF
	BREQ ULTIMORECURSO2
REGRESO:
	CPI R20, 0XFF
	BRNE UNDERFLOWROJO
	LDI R20, 0X0F
	ADD R20, R22
UNDERFLOWROJO: 
	OUT PORTD, R20 //se mandan los datos
	MOV R25, R20
    RET
ULTIMORECURSO:
	LDI R20, 0X00
	ADD R20, R22
	RJMP OVERFLOWROJO
ULTIMORECURSO2:
	LDI R20, 0XFF
	RJMP REGRESO

//Establecemos la subrutina del contador 1
ContadorAzul:
	SBRS R17, 0 //si el bit 0 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    INC R19 //se realiza el incremento
	CPI R19, 0x10
	BRNE OVERFLOWAZUL
	LDI R19, 0x00
OVERFLOWAZUL: 
	SBRS R17, 1   ////si el bit 1 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    DEC R19 //se realiza el decremento 
	CPI R19, 0XFF
	BRNE UNDERFLOWAZUL
	LDI R19, 0X0F
UNDERFLOWAZUL:
	OUT PORTC, R19 //se mandan los datos 
    RET
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