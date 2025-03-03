/*
 * Proyecto_1.asm
 *
 * Creado: 01-Mar-25 21:26 PM 
 * Autor: Rodrigo Estrada
 */

 
.include "M328PDEF.inc"
.equ SIZE = 12
.def MODO = R18 ;Aqui guardaremos el estado de nuestro reloj
.def CONT_TIMER0 = R19 ;Aqui guardaremos la cuenta para llegar a un segundo
.def SALIDA_X = R20 ;Aqui se guardara a lo que este apuntando el registro X
.def SALIDA_Y = R21 ;Aqui se guardara a lo que este apuntando el registro Y
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


	HORAS_RELOJ: .DB 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xFF, 0X00 ;Tabla con los valores del reloj 
	LDI ZH, HIGH(HORAS_RELOJ << 1) ;Cargamos desde la flash al registro Z la parte alta de HORAS_RELOJ
	LDI ZL, LOW(HORAS_RELOJ << 1) ; Cargamos desde la flash al registro Z la parte baja de HORAS_RELOJ
	LDI XH, 0X01 
	LDI XL, 0X00 ;Apuntamos a la direccion 0x0100 con el registro x de la memoria RAM 
	LDI R16, SIZE ; Cargamos el valor de size a  R16 para poder crear un bucle para poder tranferir todos los valores de HORAS_RELOJ a RAM 

//----Bucle para poder copiar a RAM----//
COPIAR: 
	LPM R17, Z+ ;sacamos el valor de Z a r20 luego avanzamos de direccion en flash
	DEC R16 ;Decrementamos r16 cada vez que se tranfiera un valor a la RAM 
	CPI R16, 0 ;Se compara para saber si ya termino el bucle
	BREQ DONE ;si no ya termino salta a DONE 
	ST X+, R17 ;Si no ha terminado, continua copiando y se mueve a la siguinte dirreccion en RAM 
	RJMP COPIAR ;regrese al inicio del bucle 
//----Termina el bucle----//
DONE:

//----Configuracion del Micro Controlador----//
SETUP: 
	CLI ;Desactivamos las interruciones globales

	//----Configuracion de frecuencia Microcontrolador----//
	LDI R16, (1 << CLKPCE) ;Habilitamos los cambios en la frecuencia del procesador
	STS CLKPR, R16 ;cargamos el enable al registro CLKPR 
	LDI R16, (1 << CLKPS0) | (1 << CLKPS2) ;Configuramos un prescaler de 32 para tener una frecuencia de 500Khz 
	STS CLKPR, R16 ;Cargamos la nueva frecuencia al registro CLKPR

	//----Configuramos el TIMER0----//
	LDI R16, (1 << CS00) | (1 << CS02) ;Configuramos el preescaler a 1024
	OUT TCCR0B, R16	;Cargamos el valor dentro de el registro encargado de hacer la modificacion
	LDI R16, 12 ;cargamos el valor desde donde empezara a contar el timer 0 para llegar a un 500ms
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
	LD SALIDA_X, X ;Apuntamos el registro X a la direccion 0x0100 donde se encuentra el valor 0 de la tabla 

	LDI YH, 0X01
	LDI YL, 0X00
	LD SALIDA_Y, Y ;Apuntamos el registro Y a la direccion 0x0100 donde se encuentra el valor 0 de la tabla

	//----Inicializamos nuestros registro----//
	CLR MODO 
	SEI ;Volvemos a habilitar las interrupciones globales
	
//----Iniciamos el Main Loop----//
LOOP:
	//----MULTIPLEXACION----//
	//----500ms----//
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTC, 0
	SBRC CONT_TIMER0, 0
	CBI PORTC, 0
	//----Aumento cada 60s----//
	CPI CONT_TIMER0, 120
	BRNE MODO_0
MODO_0:
	CPI MODO, 0
	BRNE MODO_1
	CBI PORTC, 5
	SBI PORTB, 3
	CPI SALIDA_X, 0
	BRNE CORREGIR
	LD SALIDA_X, X+
CORREGIR:
	LD SALIDA_X, X+
MODO_1:
	CPI MODO, 1
	BRNE MODO_2
	CBI PORTB, 3
	SBI PORTB, 4
	//AQUI SE VA A DESPLEGAR LA FECHA
	RJMP LOOP 
MODO_2:
	CPI MODO, 2 
	BRNE MODO_3
	CBI PORTB, 4
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTB, 3
	SBRC CONT_TIMER0, 0
	CBI PORTB, 3
	//configuracion hora
	RJMP LOOP 
MODO_3:
	CPI MODO, 3
	BRNE MODO_4
	CBI PORTB, 3
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTB, 4
	SBRC CONT_TIMER0, 0
	CBI PORTB, 4
	//CONFIGURACION DE FECHA 
	RJMP LOOP 
MODO_4:
	CPI MODO, 4
	BRNE MODO_5
	CBI PORTB, 4
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTB, 5
	SBRC CONT_TIMER0, 0
	CBI PORTB, 5
	//CONFIGURACION ALARMA
	RJMP LOOP 
MODO_5:
	CPI MODO, 5
	BRNE FIN
	CBI PORTB, 5
	//AQUI SE VA A APAGAR LA ALARMA
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTC, 5
	SBRC CONT_TIMER0, 0
	CBI PORTC, 5
FIN:
	RJMP LOOP

//----Rutina de interrupcion TIMER0----//
RI_RELOJ:
	LDI R16, 12 ;Recargamos el valor al TCNT0 
	OUT TCNT0, R16 ;Se carga el valor de 5 para que vuelva a contar 1ms 
	INC CONT_TIMER0 ;Incrementamos el regsitro que llevara la cuenta para cambiar cada 500ms 
	RETI ;Regresamos de la interrupcion

//----Rutina de interrupcion PIN CHANGE----//
RI_BOTONES:
	IN R16, PINB ;Hacemos lectura de los botones
	SBRC R16, 0 ;Si el bit 0 esta en 0 significa que se presiono el boyon de cambiar de modo
	RJMP BIT_1 ;Si no es asi sale
	INC MODO
BIT_1:	
	SBRC R16, 1 
	RJMP BIT_2
	LD	SALIDA_X, X+
BIT_2:
	SBRC R16, 2
	RJMP COMPARACION
	LD SALIDA_Y, Y+
COMPARACION:
	CPI MODO, 6 
	BRNE EXIT 
	CLR MODO
EXIT:
	RETI 








