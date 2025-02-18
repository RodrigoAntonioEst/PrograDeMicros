/*
 * Prelab2.asm
 *
 * Creado: 11-Feb-25 11:10 AM
 * Autor: Rodrigo Estrada
 */
//cargamos los datos a la flash
.include "M328PDEF.inc"
.cseg
.org 0x0000  
.equ SIZE = 17 // Esto se hace para poder finalizar el traslado de flash ya que contamos con 16 bytes 
MYTABLE: .DB 0B01000000, 0B01111001, 0B00100100, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E 
LDI ZH, HIGH(MYTABLE << 1) 
LDI ZL, LOW(MYTABLE << 1)
LDI XH, 0X02
LDI XL, 0X01
LDI R21, SIZE //Hacemos la comparacion para poder parar el traslado. 
COPIAR:
	LPM R22, Z+
	DEC R21
	CPI R21, 0
	BREQ DONE
	ST X+, R22
	RJMP COPIAR

DONE:
LDI ZH, HIGH(MYTABLE << 1)
LDI ZL, LOW(MYTABLE << 1)
LDI YH, 0X01
LDI YL, 0X01
LDI R21, SIZE //Hacemos la comparacion para poder parar el traslado.

COPIARY:
	LPM R24, Z+ 
	DEC R21
	CPI R21, 0
	BREQ DONE2
	ST Y+, R24
	RJMP COPIARY

DONE2: 

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

	//configuramos el PORTD como una salida 
	LDI R16, 0XFF
	OUT DDRD, R16 //Estabelcemos el PORTD como una salida 
	LDI R16, 0X00 
	OUT PORTD, R16//Apagamos todos los leds del display 

	//Configuramos el portB como entradas
	LDI R16, 0b00000100
	OUT DDRB, R16 //Establecemos el port B como entrada 
	LDI R16, 0b11111011
	OUT PORTB, R16 //Habilitamos todos los pull-ups 

	//guardamos datos para poder hacer el antirebote\
	LDI R18, 0X01 
	LDI XL, 0X00
	LDI XH, 0X02
	ST X, R18

	LDI R18, 0XFF 
	LDI XL, 0X11
	LDI XH, 0X02
	ST X, R18

	//Cargamos un valor para el overflow de Y
	LDI R18, 0XFF
	LDI YL, 0X11
	LDI YH, 0X01
	ST Y, R18


	//Apuntamos a la dirrecion donde se encuntran nuestros datos transferidos 
	LDI XH, 0X02
	LDI XL, 0X01
	LD	R20, X
	OUT PORTD, R20

	//APUNTAMOS A LA DIRRECION DONDE TRANFERIMOS Y
	LDI YL, 0X01
	LDI YH, 0X01
	LD R24, Y 


	LDI R17, 0XFF //Guardamos los estados de nuestros botones
	LDI R18, 0x00 //Aqui guardamos nuestro conteo de de bits para los leds 
	LDI R30, 0X00
	LDI R29, 0X00

//Programa principal
LOOP:
	CALL CONTADOR_100MS
	CALL COMPARADOR
	CALL DISPLAY7SEG
	RJMP LOOP 
//SUBRUTINA DE COMPARACION 
COMPARADOR:
	CP R24, R20 
	BREQ RESET
	CBI	PINB , 2
	RET
RESET:
	LDI YL, 0X01
	LDI YH, 0X01
	LD	R24, Y
	CLR R18
	OUT PORTC, R18
	SBI PINB, 2
	RET
//Subrutina de display de 7 segmentos 
DISPLAY7SEG:
	IN R16, PINB  //hacemos una lectura en los botones
	CP R16, R17  //Usamos esta funcion para comparar si lo que esta en R17 y R16 es lo mismo
	BRNE SALTO
	RET //si es lo mismo se regresa al loop inicial
SALTO:
	CALL DELAY
	IN R16, PINB  //se leen de nuevo los botones, luego de cierta cantidad de tiempo 
    CP R16, R17  //si al hacer la comparacion se detecta que el push sigue en el mismo estado se manda de regreso al loop
	BRNE SALTO2
    RET	//si es igual manda de regreso al loop
SALTO2:
	MOV R17, R16 //se guarda el nuevo estado
	SBRC R17, 0 //Analizamos si tenemos que desplazarnos hacia adelante
	RJMP ATRAS
	LD R20, X+
	CPI R20, 0XFF
	BRNE ATRAS
	LDI XL, 0X01
	LDI XH, 0X02
	LD	R20, X
ATRAS:
	SBRC R17, 1 //Analizamos si tenes que deplzarnos hacia atras 
	RJMP FIN
	LD R20, -X
	CPI R20, 0X01
	BRNE FIN
	LDI XL, 0X10
	LDI XH, 0X02
	LD R20, X 
FIN:
	OUT PORTD, R20
	RET 

//Subrutina del contador cada 100ms 
CONTADOR_100MS:
	IN R16, TIFR0 //Leemos el registro TIFR0 para ver si si se llego al valor deseada para interrumpir el timer0
	SBRS R16, TOV0 //esta linea verifica si la bandera de overflow esta activa, y hasta que esta se activa salta.
	RET
	SBI TIFR0, TOV0 //limpiamos la bandera de overflow, le cargamos un 1 para descativarla
	LDI R16, 0x9E
	OUT TCNT0, R16 //Recargamos el valor al timer0 cada vez que haga overflow 
	INC R30
	CPI R30, 0X0A
	BRNE FINCONTADOR
	CLR R30
	INC R18 //Incrementamos cada vez que se llega a los 100ms
	LD R24, Y+
	CPI R24, 0XFF
	BRNE NOTOVERFLOW
	LDI YL, 0X01
	LDI YH, 0X01
	LD	R24, X
NOTOVERFLOW:
	CPI R18, 0X10
	BRBS 1, OVERFLOW
	OUT PORTC, R18 //sacamos a PORTC lo que tengo guardado en r18
	RET
OVERFLOW:
	LDI R18, 0X00
	OUT PORTC, R18
FINCONTADOR:
	RET

//subrutina de interrupcion 
DELAY: //usamos la funcion delay para poder evitar rebotes en los push 
	LDI R25, 0
SUBDELAY1: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa 
	INC R25
	CPI R25, 0
	BRNE SUBDELAY1
	LDI R25, 0
SUBDELAY2://el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R25
	CPI R25, 0
	BRNE SUBDELAY2
	LDI R25, 0
SUBDELAY3: //el contador en este delay incrementa hasta 255 y luego pasa a la siguiente etapa
	INC R25
	CPI R25, 0
	BRNE SUBDELAY3
	RET
