/*
 * Proyecto_1.asm
 *
 * Creado: 01-Mar-25 21:26 PM 
 * Autor: Rodrigo Estrada
 */

 
.include "M328PDEF.inc"
.equ SIZE = 12
.def MODO = R18 ;Aqui guardaremos el estado de nuestro reloj
//-----ininicalizamos los vectores-----//
.cseg
.org 0x0000
	RJMP START ;Inicializamos la configuracion del programa. 
.org PCI0addr
	RJMP RI_BOTONES ;Colocamos el vector de interrupcion de pin change 
.org OVF0addr
	RJMP RI_RELOJ ;Colocamos la rutina de interrupcion de timer 0 overflow

//----Configuracion de la pila ----//
START:
	LDI R16, LOW(RAMEND) ;Cargamos el byte bajo de RAMEND en r16
	OUT SPL, R16	;inicializar el registro de pila bajo (SPL)
	LDI R16, HIGH(RAMEND) ;Cargar ek byte alto de  RAMEND en R16
	OUT SPH, R16 ;Inicializar el registro de pila alto (SPH)


	HORAS_RELOJ: .DB 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xFF ;Tabla con los valores del reloj 
	LDI ZH, HIGH(HORAS_RELOJ << 1) ;Cargamos desde la flash al registro Z la parte alta de HORAS_RELOJ
	LDI ZL, LOW(HORAS_RELOJ << 1) ; Cargamos desde la flash al registro Z la parte baja de HORAS_RELOJ
	LDI XH, 0X01 
	LDI XL, 0X00 ;Apuntamos a la direccion 0x0100 con el registro x de la memoria RAM 
	LDI R16, SIZE ; Cargamos el valor de size a  R16 para poder crear un bucle para poder tranferir todos los valores de HORAS_RELOJ a RAM 

//----Bucle para poder copiar a RAM----//
COPIAR: 
	LPM COPIAR, Z+ ;sacamos el valor de Z a r20 luego avanzamos de direccion en flash
	DEC R16 ;Decrementamos r16 cada vez que se tranfiera un valor a la RAM 
	CPI R16, 0 ;Se compara para saber si ya termino el bucle
	BREQ DONE ;si no ya termino salta a DONE 
	ST X+, R16 ;Si no ha terminado, continua copiando y se mueve a la siguinte dirreccion en RAM 
	RJMP COPIAR ;regrese al inicio del bucle 
//----Termina el bucle----//
DONE:

//----Configuracion del Micro Controlador----//
SETUP: 
	CLI ;Desactivamos las interruciones globales

	//----Configuracion de frecuencia Microcontrolador----//
	LDI R16, (1 << CLKPCE) ;Habilitamos los cambios en la frecuencia del procesador
	STS CLKPR, R16 ;cargamos el enable al registro CLKPR 
	LDI R16, (1 << CLKPS00) | (1 << CLKPS02) ;Configuramos un prescaler de 32 para tener una frecuencia de 500Khz 
	STS CLKPR, R16 ;Cargamos la nueva frecuencia al registro CLKPR

	//----Configuramos el TIMER0----//
	LDI R16, (1 << CS00) | (1 << CS02) ;Configuramos el preescaler a 1024
	OUT TCCR0B, R16	;Cargamos el valor dentro de el registro encargado de hacer la modificacion
	LDI R16, 12 ;cargamos el valor desde donde empezara a contar el timer 0 para llegar a 500 ms
	OUT TCNT0, R16 ;Cargamos el valor al registro para darnos el tiempo deseado.

	//----Configuracion de interrupcion de PIN CHANGE----//
	LDI R16, (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2)  ;Configuramos el pinchage para PB0, PB1 y PB2
	STS PCMSK0, R16 ; lo cargamos al registro de la mascara
	LDI R16, (1 << PCIE0) ;Cargamos el bit que habilita el pinchange en PORTB
	STS PCICR, R16 ;habilitamos el pin change el PORTB 

	//----Configuramos la interrupcion del TIMER0----//
	LDI R16, (1 << TOIE0) ;Habilitamos las interrupciones de timer 0
	STS TIMSK0, R16  ;ke cargamos al BIT la mascara 

	//----INICIAMOS CON LA CONFIGURACION DE PUERTOS----//
	
	//----PORTD----//
	LDI R16, 0XFF 
	OUT DDRD, R16 ;Configutamo PORTD como una salida
	LDI R16, 0X00 
	OUT PORTD, R16 ;Apagamos las salidas de PORTD
	
	//----PORTB----//
	LDI R16, 0B11111000
	OUT DDRB, R16 ;Se configuran PB0 - PB2 como entradas y el resto como salidas 
	LDI R16, 0B00000111 ;Se configuran los PULLUPS de PB0 - PB2 y el resto se apagan las salidas 
	OUT PORTB, R16 
	
	//----PORTC----//
	LDI R16, 0XFF
	OUT DDRC, R16 ;Se configura el puerto con todas como salidas 
	LDI R16, 0X00
	OUT PORTC, R16 ;Apagamos todas las salidas
	
	//----Apuntamos los punteros al inicio de los datos----//
	LDI XH, 0X01
	LDI XL, 0X00
	ST X, R16 ;Apuntamos el registro X a la direccion 0x0100 donde se encuentra el valor 0 de la tabla 

	LDI YH, 0X01
	LDI YL, 0X00
	ST Y, R17 ;Apuntamos el registro Y a la direccion 0x0100 donde se encuentra el valor 0 de la tabla

	//----Inicializamos nuestros registro----//
	CLR MODO 

	SEI ;Volvemos a habilitar las interrupciones globales

//----Iniciamos el Main Loop----//
LOOP:


	 









