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
	LDI R16, (1 << CLKPCE)  //Cargar el bit CLKPCE en R16
	STS CLKPR, R16          //Habilitar el cambio del prescaler
	LDI R16, 0b00000100     //Cargar el valor del prescaler (división por 16)
	STS CLKPR, R16          //Escribir el nuevo valor en el registro CLKPR

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
	LDI R21, 0X00 //registros para hacer copias 
	LDI R22, 0X00 //registros para hacer copias
	LDI R23, 0X00 //registros para hacer copias
	

 // Loop principal
LOOP:
	CALL Antirebote //subrutina de antirebote 
    CALL ContadorAmarillo //subrutina de contador 1
	CALL ContadorBlanco //subrutina de contador 2
    CALL Suma //subrutina de suma 
	RJMP LOOP //de regreso al loop


//subrutina de suma con carry 
Suma:
    SBRC R17, 4     //Si el bit 4 de R17 es 0, sigue. Si es 1, salta a RETORNO
    RJMP RETORNO	//si no se cumple la condicion se salta todo el proceso de suma
	ADD R21, R22	//se suma, si en la suma existe un carry, se mostrara ya que agarre todo el puerto C como mi salida del sumador 
	OUT PORTC, R21	//se saca el valor del de r21 al portC 
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
ContadorAmarillo:
	SBRS R17, 2 //si el bit 2 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    INC R20 //se realiza el incremento
	CPI R20, 0X10 //se compara con 0x10 para saber si existe overflow en el primer contador y asi regresar la cuenta
	BRNE OVERFLOWROJO //si no son iguales salta el comando que carga el valor que quita el overflow 
	LDI R20, 0X00
OVERFLOWROJO:
	SBRS R17, 3   //si el bit 3 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    DEC R20 //se realiza el decremento
	CPI R20, 0XFF //se compara con oxff para saber si existe un underflow y asi regresarlo al valor 0x00
	BRNE UNDERFLOWROJO //si no son iguales salta la operacion que quita el underflow
	LDI R20, 0X0F
UNDERFLOWROJO:
	MOV R25, R20 //se copia en este registro para poder guardar luego juntar el valor del contador 1 y el 2 ya que estan en el mismo puerto
	MOV R21, R20 //guardo este valor en este registro para poder hacer la suma entre contadores
	OUT PORTD, R25 //se mandan los datos al portD 
    RET


//Establecemos la subrutina del contador 1
ContadorBlanco:
	SBRS R17, 0 //si el bit 0 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    INC R19 //se realiza el incremento
	CPI R19, 0x10 //se compara con 0x10 para saber si existe overflow en el segundo contador y asi regresar la cuenta
	BRNE OVERFLOWAZUL ////si no son iguales salta el comando que carga el valor que quita el overflow
	LDI R19, 0x00
OVERFLOWAZUL: 
	SBRS R17, 1   ////si el bit 1 de R17 esta en "1" este se saltara la siguiente instruccion, esto debido a los pullups, cuando el boton este precionado este mandara un 0
    DEC R19 //se realiza el decremento 
	CPI R19, 0XFF //se compara para quitar en el under flow
	BRNE UNDERFLOWAZUL //si no existe underflow salta la accion que salta la carga del valor que quita el underflow 
	LDI R19, 0X0F
UNDERFLOWAZUL:
	MOV R23, R19 //se copia este valor para poder poder juntar ambos contadores en el mismo puerto
	MOV R22, R19 //se mueve este valor para poder hacer la suma 
	SWAP R23 //se usa swap para poder mover los 4 bits menos significativos a los 4 bits mas significativos y asi poder juntar ambos contadores sin que estos interfieran entre si 
	OR R25, R23 //El or lo que permite es poder juntar el valor de ambos contadores y poder mostrarlos en el portD
	OUT PORTD, R25 //se mandan los datos al portD
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