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
//.def SALIDA_X = R20 ;Aqui se guardara a lo que este apuntando el registro X
.def UNIDADES_MINUTOS = R21 
.def DECENAS_MINUTOS = R22
.def UNIDADES_HORAS = R23 
.def DECENAS_HORAS = R24
.def DISPLAYUNO = R25 
.def DISPLAYDOS = R28 
.def DISPLAYTRES = R29 
.def DISPLAYCUATRO = R30
.def CONT_TIMER2 = R31

//-----ininicalizamos los vectores-----//
.cseg
.org 0x0000
	RJMP START ;Inicializamos la configuracion del programa. 
.org PCI0addr
	RJMP RI_BOTONES ;Colocamos el vector de interrupcion de pin change  
.org OVF2addr
	RJMP RI_10MS ;Rutina de interrupcion de 10 MS
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

	//----Configuramos el TIMER2----//
	LDI R16, (1 << CS20) | (1 << CS21) | (1 << CS22) ;Colocamos un prescaler de 1024
	STS TCCR2B, R16 ;Lo cargamos al registro que se encarga de hacer las modificaciones. 
	LDI R16, 254 ; cada 5ms 
	STS TCNT2, R16 ;Cargamos el valor para que empiece a contar desde ese punto para tener 10ms 

	//----Configuracion de interrupcion de PIN CHANGE----//
	LDI R16, (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2)  ;Configuramos el pinchage para PB0, PB1 y PB2
	STS PCMSK0, R16 ; lo cargamos al registro de la mascara
	LDI R16, (1 << PCIE0) ;Cargamos el bit que habilita el pinchange en PORTB
	STS PCICR, R16 ;habilitamos el pin change el PORTB 

	//----Configuramos la interrupcion del TIMER0----//
	LDI R16, (1 << TOIE0) ;Habilitamos las interrupciones de timer 0
	STS TIMSK0, R16  ;ke cargamos al BIT la mascara 

	//----Configuramos interrupcion del timer2----//
	LDI R16, (1 << TOIE2) ;Habilitamos las interrupciones del timer 2
	STS TIMSK2, R16 ;le cargamos a la macara 

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

	//----Inicializamos nuestros registro----//
	CLR MODO 
	CLR UNIDADES_MINUTOS
	CLR DECENAS_MINUTOS
	CLR UNIDADES_HORAS
	CLR DECENAS_HORAS
	CLR DISPLAYUNO
	CLR DISPLAYDOS
	CLR DISPLAYTRES 
	CLR DISPLAYCUATRO 
	CLR CONT_TIMER2



	SEI ;Volvemos a habilitar las interrupciones globales

	//----Cargamos el 0 en todos los registros----//
	LD DISPLAYUNO, X
	LD DISPLAYDOS, X
	LD DISPLAYTRES, X
	LD DISPLAYCUATRO, X
	
//----Iniciamos el Main Loop----//
LOOP:
	CPI CONT_TIMER2, 4 ;comparamo si ya es 5 
	BRNE FIN_TIMER ;si no es igual lo sacamos de la interrupcion 
	CLR CONT_TIMER2 ;si es igual lo limpiamos
FIN_TIMER:
	//----Aumento cada 60s----//
	CPI CONT_TIMER0, 1
	BRNE MODO_0
	CLR CONT_TIMER0
//----Desactivacion de conteo automomatico----// 
	CPI MODO, 2
	BRNE CUENTA_AUTOMATICA
	RJMP MODO_2
CUENTA_AUTOMATICA:
//----UNIDADES MINUTOS----//
	LDI XH, 0X01
	LDI XL, 0X00 
	INC UNIDADES_MINUTOS
	CPI UNIDADES_MINUTOS, 10
	BREQ DECENAS_MIN
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	RJMP MODO_0

DECENAS_MIN:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR	UNIDADES_MINUTOS
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	INC DECENAS_MINUTOS
	CPI DECENAS_MINUTOS, 6 
	BREQ UNIDADES_HOR
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYDOS, X
	RJMP MODO_0
UNIDADES_HOR:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR DECENAS_MINUTOS
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYDOS, X
	INC UNIDADES_HORAS
	CPI UNIDADES_HORAS, 4
	BREQ CASO_ESPECIAL
REGRESO:
	CPI UNIDADES_HORAS, 10 
	BREQ DECENAS_HOR
	ADD XL, UNIDADES_HORAS
	LD DISPLAYTRES, X
	RJMP MODO_0   
CASO_ESPECIAL:
	CPI DECENAS_HORAS, 2
	BREQ FIN_RELOJ
	RJMP REGRESO
DECENAS_HOR:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR UNIDADES_HORAS
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYTRES, X
	INC DECENAS_HORAS 
	ADD XL, DECENAS_HORAS
	LD DISPLAYCUATRO, X
	RJMP MODO_0
FIN_RELOJ:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR DECENAS_HORAS
	CLR UNIDADES_HORAS
	ADD XL, DECENAS_HORAS
	LD DISPLAYCUATRO, X
	ADD XL, UNIDADES_HORAS
	LD DISPLAYTRES, X 
	LD DISPLAYDOS, X
	LD DISPLAYUNO, X
MODO_0:
	CPI MODO, 0 ;Comparamos para ver si estamos en el modo correcto.
	BRNE MODO_1	;si no lo mandamos a comparar a al siguente modo
	//----500ms----// 
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTC, 0
	SBRC CONT_TIMER0, 0
	CBI PORTC, 0
	CBI PORTC, 5 ;apagamos en pin 5 de PORTC por si estaba encendido al salir del modo 5 
//----indicador de hora----//
	SBI PORTB, 3 ;encendemos el indicador de modo hora 
//----Inicio de la multiplexacion----//
	CALL MULTIPLEXACION 
	RJMP LOOP

MODO_1:
	CPI MODO, 1
	BRNE MODO_2
	CBI PORTB, 3
	SBI PORTB, 4
	
	RJMP LOOP 

MODO_2:
	CPI MODO, 2 
	BRNE MODO_3
	CBI PORTB, 4
//----INDICADOR MODO CONFIG HORA----//
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTB, 3
	SBRC CONT_TIMER0, 0
	CBI PORTB, 3
	CBI PORTC, 0
//---cambio de hora manual---//
	CALL DECENAS_UNIDADES_MIN
	CALL UNIDADES_DECENAS_HOR
	CALL MULTIPLEXACION 
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

//----Rutina de interrupcion del TIMER2----//
RI_10MS:
	LDI R16, 254 ;Recargamos el valor de del TCNT2
	STS TCNT2, R16 ;Se recarga el valor de 251 
	INC CONT_TIMER2 ;Incrementamos el registro cada 10ms 
	RETI 

//----Rutina de interrupcion PIN CHANGE----//
RI_BOTONES:
	IN R16, PINB ;Hacemos lectura de los botones
	SBRC R16, 0 ;Si el bit 0 esta en 0 significa que se presiono el boyon de cambiar de modo
	RJMP BIT_1 ;Si no es asi sale
	INC MODO
BIT_1:	
	SBRC R16, 1 
	RJMP BIT_2
	INC UNIDADES_MINUTOS 
BIT_2:
	SBRC R16, 2
	RJMP COMPARACION
	INC UNIDADES_HORAS
COMPARACION:
	CPI MODO, 6 
	BRNE EXIT 
	CLR MODO
EXIT:
	RETI
//----SUBRUTINA DE MULTIPLEXACION
MULTIPLEXACION:
	CPI CONT_TIMER2, 0
	BRNE DISPLAY2 
	SBI PORTC, 1
	OUT PORTD, DISPLAYUNO
	CBI PORTC, 1
DISPLAY2: 
	CPI CONT_TIMER2, 1
	BRNE DISPLAY3
	SBI PORTC, 2
	OUT PORTD, DISPLAYDOS
	CBI PORTC, 2
DISPLAY3:
	CPI CONT_TIMER2, 2
	BRNE DISPLAY4
	SBI PORTC, 4
	OUT PORTD, DISPLAYTRES
	CBI PORTC, 4 
DISPLAY4:
	CPI CONT_TIMER2, 3
	BRNE FIN_MULTI
	SBI PORTC, 3
	OUT PORTD, DISPLAYCUATRO 
	CBI PORTC, 3 
FIN_MULTI:
	RET

//----SUB-RUTINA DE DECENAS Y UNIDADES DE MINUTOS----//
DECENAS_UNIDADES_MIN:
	;CAMBIO DE UNIDADES MINUTOS
	LDI XH, 0X01
	LDI XL, 0X00
	CPI UNIDADES_MINUTOS, 10
	BRSH DECENAS_MIN_MANUAL
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	RET
DECENAS_MIN_MANUAL:
`	LDI XH, 0X01
	LDI XL, 0X00
	CLR	UNIDADES_MINUTOS
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	INC DECENAS_MINUTOS
	CPI DECENAS_MINUTOS, 6 
	BRSH OVERFLOW_UNIDADES_MIN 
	CLR R26 
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYDOS, X
	RET
OVERFLOW_UNIDADES_MIN:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR DECENAS_MINUTOS
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYDOS, X
	RET

//----RUTINA DECENAS UNIDADES HORAS----//
UNIDADES_DECENAS_HOR:
	LDI XH, 0X01
	LDI XL, 0X00
	CPI UNIDADES_HORAS, 4
	BREQ CASO_ESPECIAL_MANUAL
REGRESO_MANUAL:
	CPI UNIDADES_HORAS, 10 
	BRSH DECENAS_HOR_MANUAL 
	ADD XL, UNIDADES_HORAS
	LD DISPLAYTRES, X
	RJMP FIN_CAMBIOS   
CASO_ESPECIAL_MANUAL:
	CPI DECENAS_HORAS, 2
	BREQ FIN_RELOJ_MANUAL
	RJMP REGRESO_MANUAL 
DECENAS_HOR_MANUAL:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR UNIDADES_HORAS
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYTRES, X 
	INC DECENAS_HORAS
	CLR R26
	ADD XL, DECENAS_HORAS
	LD DISPLAYCUATRO, X
	RJMP FIN_CAMBIOS
FIN_RELOJ_MANUAL:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR R27
	CLR UNIDADES_MINUTOS 
	CLR DECENAS_MINUTOS
	CLR DECENAS_HORAS
	CLR UNIDADES_HORAS
	ADD XL, DECENAS_HORAS
	LD DISPLAYCUATRO, X
	ADD XL, UNIDADES_HORAS
	LD DISPLAYUNO, X
	LD DISPLAYTRES, X 
	LD DISPLAYDOS, X
	LD DISPLAYUNO, X
	RJMP FIN_CAMBIOS
FIN_CAMBIOS:
	RET