/*
 * Proyecto_1.asm
 *
 * Creado: 01-Mar-25 21:26 PM 
 * Autor: Rodrigo Estrada
 */

 
.include "M328PDEF.inc"
.equ SIZE = 12 ;Definimos cuantas veces queremos que haga el bucle de copiar de flash a ram.
.def MODO = R18 ;Aqui guardaremos el estado de nuestro reloj
.def CONT_TIMER0 = R19 ;Aqui guardaremos la cuenta para llegar a un segundo
//.def SALIDA_X = R20 ;Aqui se guardara a lo que este apuntando el registro X

//----Establecemos los registros para incrementar la hora----//
.def UNIDADES_MINUTOS = R21 
.def DECENAS_MINUTOS = R22	
.def UNIDADES_HORAS = R23 
.def DECENAS_HORAS = R24
//----Establcemos los registros para desplegar la hora----//
.def DISPLAYUNO = R25 
.def DISPLAYDOS = R28 
.def DISPLAYTRES = R29 
.def DISPLAYCUATRO = R30
//----Registro que lleva el conteo de TIMER2----//
.def CONT_TIMER2 = R31
//----Registros para poder aumentar la fecha----//
.def UNIDADES_DIAS = R15
.def DECENAS_DIAS = R14
.def UNIDADES_MES = R13
.def DECENAS_MES = R12 
//----Registros para poder desplegar la fecha----//
.def DISPLAYUNO_FECHA = R11
.def DISPLAYDOS_FECHA = R10
.def DISPLAYTRES_FECHA = R9
.def DISPLAYCUATRO_FECHA = R8
//----registros para poder desplegar la alarma----//
.def ALARMA_MINUTOS_UNIDADES = R7
.def ALARMA_MINUTOS_DECENAS = R6
.def ALARMA_HORAS_UNIDADES = R5
.def ALARMA_HORAS_DECENAS = R4
//----registros para poder mostrar la alarma manualmente----//
.def ALARMA_DISPLAYUNO = R3
.def ALARMA_DISPLAYDOS = R2
.def ALARMA_DISPLAYTRES = R1
.def ALARMA_DISPLAYCUATRO = R0

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
	CLR DISPLAYUNO_FECHA
	CLR DISPLAYDOS_FECHA
	CLR DISPLAYTRES_FECHA
	CLR DISPLAYCUATRO_FECHA
	CLR UNIDADES_DIAS
	INC UNIDADES_DIAS 
	CLR DECENAS_DIAS 
	CLR UNIDADES_MES 
	INC UNIDADES_MES 
	CLR DECENAS_MES 
	CLR ALARMA_MINUTOS_UNIDADES 
	CLR ALARMA_MINUTOS_DECENAS 
	CLR ALARMA_HORAS_UNIDADES
	CLR ALARMA_HORAS_DECENAS
	CLR ALARMA_DISPLAYUNO
	CLR ALARMA_DISPLAYDOS 
	CLR ALARMA_DISPLAYTRES
	CLR ALARMA_DISPLAYCUATRO

	SEI ;Volvemos a habilitar las interrupciones globales

	//----Cargamos el 0 en todos Los displays para la hora----//
	LD DISPLAYUNO, X
	LD DISPLAYDOS, X
	LD DISPLAYTRES, X
	LD DISPLAYCUATRO, X

	//----inicializamos todos los displays para la fecha----//
	LDI XL, 0X01 //le cargamos 0x01 al XL para que nuestra fecha inicie en 01/01
	LD DISPLAYUNO_FECHA, X
	LDI XL, 0X00
	LD DISPLAYDOS_FECHA, X
	LDI XL, 0X01 //le cargamos 0x01 al XL para que nuestra fecha inicie en 01/01
	LD DISPLAYTRES_FECHA, X
	LDI XL, 0X00
	LD DISPLAYCUATRO_FECHA, X

	//----INICIALIZAMOS LOS DISPLAYS DE LA ALARMA----//
	LD ALARMA_DISPLAYUNO, X
	LD ALARMA_DISPLAYDOS, X
	LD ALARMA_DISPLAYTRES, X 
	LD ALARMA_DISPLAYCUATRO, X ; esto se hace para poder sacar el valor 0 en el display cuando se conecta el atmega328p
	
//----Iniciamos el Main Loop----//
LOOP:
//----logica de para multiplexar con timer2----//

	CPI CONT_TIMER2, 4 ;incrementara 1 unidad cada 5ms, para saber si ya pasaron los 20ms se compara con 4 y luego se limpia. 
	BRNE FIN_TIMER ;si no es igual lo sacamos de la interrupcion 
	CLR CONT_TIMER2 ;si es igual lo limpiamos
FIN_TIMER:
	//----Aumento cada 60s----//
	CPI CONT_TIMER0, 120 ;este es la logica que permite incrementar cada 60s el display de minutos
	BRNE MODO0 ;Si no cuenta 60s salta al modo0 para comparar en que modo se encuetra 
	CLR CONT_TIMER0 ; si ya pasaron los 60s se manda a la logica que permite incrementar cada 60s outomaticamente
	RJMP DESACTIVACION ; hace un salto a una condicion previa, la cual permite mostrar que cuente en todos los modos eceptuando en config hora 
MODO0:
	RJMP MODO_0 ;Salta a hacer las comparaciones de modo
//----Desactivacion de conteo automomatico----// 
DESACTIVACION:
	CPI MODO, 2 ;compar si se encuentra en el modo 2 y no permitir la cuenta automatica
	BRNE CUENTA_AUTOMATICA ;si no es igual al modo 2 salta a contar automaticamente
	RJMP MODO_0 ;si fuera igual se va a comparar modos
CUENTA_AUTOMATICA:
//----UNIDADES MINUTOS----//
	LDI XH, 0X01
	LDI XL, 0X00 ;se ponen los punteros al inicio de la tabla 
	INC UNIDADES_MINUTOS ;incrementa unidades de minutos cada 60 segundos
	CPI UNIDADES_MINUTOS, 10 ;compara si no existe overflow 
	BREQ DECENAS_MIN	;si existe overflow se manda a resetear
	ADD XL, UNIDADES_MINUTOS	;si no existe si deplazada en la tabla al numero deseado 
	LD DISPLAYUNO, X ;se carga en el display y se muestra al usuario
	RJMP MODO_0 ;Se va a modo 0 para comparar en que modo esta

DECENAS_MIN:
	LDI XH, 0X01
	LDI XL, 0X00 ;se ponen los punteros al inicio de la tabla
	CLR R26 ;se limpia el lo que existe en R26
	CLR	UNIDADES_MINUTOS ;Limpiamos unidades de minutos por overflow
	ADD XL, UNIDADES_MINUTOS 
	LD DISPLAYUNO, X ;cargamos el valor limpio
	INC DECENAS_MINUTOS ;incrementamos las decenas de minutos para poder hacer overflow
	CPI DECENAS_MINUTOS, 6 ;comparamos para ver si existe overflow
	BREQ UNIDADES_HOR	;si existe overflow se manda a unidades horas para limpiar el display 2
	ADD XL, DECENAS_MINUTOS ;se apunta al numero de la tabla que deseamos mostrar
	LD DISPLAYDOS, X	;cargamos a display 2 el valor de x
	RJMP MODO_0	;mandamos a comparar a modos
UNIDADES_HOR:
	LDI XH, 0X01
	LDI XL, 0X00 ;apuntamos al inicio de la tabla
	CLR DECENAS_MINUTOS	;limpiamos las decenas de minutos por overflow
	ADD XL, DECENAS_MINUTOS	
	LD DISPLAYDOS, X	;se carga el valor limpio al display 
	INC UNIDADES_HORAS	;incrementamos las unidades de las horas 
	CPI UNIDADES_HORAS, 4	;comparamos si existe overflow de 24 horas 
	BREQ CASO_ESPECIAL	;si es igual saltamos a caso espacial para ver que no se haya pasado de las 23 horas 
REGRESO:
	CPI UNIDADES_HORAS, 10 ;aqui si no se ha pasado de las 23 horas mira si no existe overflow de decenas de horas
	BREQ DECENAS_HOR	;si son iguales manda a overflow
	ADD XL, UNIDADES_HORAS	; si no son iguales apunta al valor que se quiere mostrar en el display
	LD DISPLAYTRES, X	;cargamos el valor deseado al display 3
	RJMP MODO_0   ;saltamos a los modos
CASO_ESPECIAL:
	CPI DECENAS_HORAS, 2	;el caso especial para ver si ya se paso de las 23 horas, si se cumple que unidades horas sean 4 y decenas horas sean 2 hace el overflow 
	BREQ FIN_RELOJ ;si no es igual se va a final de la logica de incrementar hora 
	RJMP REGRESO ;si no se cumplio se regresa al flujo normal de analisis de overflow para las unidades de horas
DECENAS_HOR:
	LDI XH, 0X01
	LDI XL, 0X00 ;apuntamos de nuevo al inicio de la tabla 
	CLR UNIDADES_HORAS ;limpiamos unidades 
	ADD XL, DECENAS_MINUTOS	
	LD DISPLAYTRES, X	;mostramos en display 3
	INC DECENAS_HORAS ;incrementamos las decenas de horas
	ADD XL, DECENAS_HORAS ;lo mandamos a moverse a la posicion que deseamos
	LD DISPLAYCUATRO, X ; se muestra en el display 4
	RJMP MODO_0
FIN_RELOJ:
	;aqui se realiza el overflow global, ya que se llego al final de las 23 horas
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

//----DESACTIVACION FECHA----//
	;esta e la logica que desactiva la fecha, si esta en el modo 3 deja de aumentar la fecha automaticamente
	CPI MODO, 3 
	BRNE FECHA
	RJMP MODO_0
//----LOGICA DE DESPLIEGUE DE FECHA----//
FECHA:
    LDI XH, 0X01 ;una vez que ya se cumplieron las 24 horas y se hacer overflow, se pasa a esta seccion de codigo que permite que avance de dia cada vez que se cumplen 24 horas
    LDI XL, 0X00; se apunta con el puntero al inicio de la tabla 
	CLR R26
    LD DISPLAYUNO_FECHA, X ;se carga el valor de x al display 
    INC UNIDADES_DIAS ;se incrementan los dias cada vez que existe overflow 
    MOV R20, UNIDADES_DIAS	;hacemos esta logica de hacer un mov al registro R20, ya que los registros utilizados para la fecha estan en los que van de R0 a R15
    CPI R20, 9	;hacemos la comparacion del overflow para febrero 
    BRSH CASO_FEBRERO	; primero analiza si esta en el caso de febrero si no esta en el caso de febrero salta para seguir el flujo normal
	RJMP REGRESO_NO_FEBRERO 

CASO_FEBRERO:
;esta logica se asegura que este en el mes de febrero y que no se pase a 29 de febrero
    MOV R20, DECENAS_DIAS
    CPI R20, 2
    BRSH REGRESO_NO_FEBRERO
	MOV R20, UNIDADES_MES 
	CPI R20, 2;Se asegura que este en el mes 2 
	BRNE REGRESO_NO_FEBRERO
	MOV R20, DECENAS_MES 
	CPI R20, 0; Tambien se asegura que las decenas de meses esten en 0 para que no interfiera con diciembre 
	BREQ UNIDADES_MESES

REGRESO_NO_FEBRERO:
;esta es la logica que permite que ciertos meses no muestren 31, 
;primero analiza si esta en 31, si esta en 31 luego manda a analizar si esta en los meses que solo tienen 30 dias 
    MOV R20, UNIDADES_DIAS
    CPI R20, 1 
    BRNE CASO_NO_TREINTA
    MOV R20, DECENAS_DIAS
    CPI R20, 3
    BRNE CASO_NO_TREINTA
    MOV R20, UNIDADES_MES
    CPI R20, 4
    BREQ UNIDADES_MESES    ; Abril
    CPI R20, 6
    BREQ UNIDADES_MESES    ; Junio
    CPI R20, 9
    BREQ DECENAS_MESES		;septiembre 
	MOV R20, UNIDADES_MES
	CPI R20, 1
	BRNE CASO_NO_TREINTA
    MOV R20, DECENAS_MES
    CPI R20, 1;				
    BREQ UNIDADES_MESES    ; Noviembre (mes 11)

CASO_NO_TREINTA:
;Si el codigo detecta que no esta en un mes que solo posee 30 meses sale de la logica para hacer el overflow
    MOV R20, UNIDADES_DIAS
    CPI R20, 2
    BRNE FLUJO_NORMAL
    MOV R20, DECENAS_DIAS
    CPI R20, 3		;Esta logica compara si ya se paso de 31 y hace el overflow
    BREQ UNIDADES_MESES

FLUJO_NORMAL:
;Esta logica permite hacer el overflow solo en las unidades de dias.
    MOV R20, UNIDADES_DIAS
    CPI R20, 10	;si detecta que se oaso de 9 este hace el overflow en el display de los dias
    BREQ DECENAS_DIA	
    ADD XL, UNIDADES_DIAS
    LD DISPLAYUNO_FECHA, X
    RJMP MODO_0
;Esta logica permite borrar lo que este en el display de unidades dias y aumentar uno en decenas dias
DECENAS_DIA:
;Esta logica permite hacer el overflow en el display de decenas de dias
    CLR R26
    LDI XH, 0X01
    LDI XL, 0X00
    CLR UNIDADES_DIAS
    INC DECENAS_DIAS
    LD DISPLAYUNO_FECHA, X
    ADD XL, DECENAS_DIAS
    LD DISPLAYDOS_FECHA, X
    RJMP MODO_0

UNIDADES_MESES:
;esta es la logica que permite hacer en overflow en el display de decenas de dias
    LDI XH, 0X01
    LDI XL, 0X00
    CLR DECENAS_DIAS
    CLR UNIDADES_DIAS ;limpia ambos displays, e 1 y el 2 y les carga el valor de 0
    ADD XL, DECENAS_DIAS
    LD DISPLAYDOS_FECHA, X
	LD DISPLAYUNO_FECHA, X
    INC UNIDADES_MES
    MOV R20, UNIDADES_MES
    CPI R20, 3	;primero compara si ya esta en tres para ver si ya se paso de 12 meses
    BREQ CASO_ESPECIAL_FECHA

REGRESO_FECHA:
;esta es la logica que permite seguir incrementando si no existe overflow de ningun tipo
    CPI R20, 10
    BREQ DECENAS_MESES
    ADD XL, UNIDADES_MES
    LD DISPLAYTRES_FECHA, X
    RJMP MODO_0
	  
CASO_ESPECIAL_FECHA:
;aqui se analiaza si ya nos pasamos de 12 meses 
    MOV R20, DECENAS_MES
    CPI R20, 1	;si se encuentra que se pasaron de 12 meses se va a hacer un overflow que limpia todos los displays
    BREQ FIN_FECHA
    RJMP REGRESO_FECHA

DECENAS_MESES:
;aqui entra solo si hubo un overflow en el display de unidades de meses 
    LDI XH, 0X01
    LDI XL, 0X00
	CLR UNIDADES_MES
	CLR UNIDADES_DIAS 
	CLR DECENAS_DIAS
    INC DECENAS_MES
	LD DISPLAYUNO_FECHA, X
	LD DISPLAYDOS_FECHA, X 
    LD DISPLAYTRES_FECHA, X
    ADD XL, DECENAS_MES
    LD DISPLAYCUATRO_FECHA, X
    RJMP MODO_0
;Esta es la logica que hace que si se paso de 12 meses todo se reinicie y le limpien todos los registros
FIN_FECHA:
    LDI XH, 0X01
    LDI XL, 0X00
    INC UNIDADES_DIAS 
	CLR DECENAS_DIAS 
	CLR UNIDADES_MES 
	INC UNIDADES_MES 
	CLR DECENAS_MES
    LDI XL, 0X01
	LD DISPLAYUNO_FECHA, X
	LDI XL, 0X00
	LD DISPLAYDOS_FECHA, X
	LDI XL, 0X01
	LD DISPLAYTRES_FECHA, X
	LDI XL, 0X00
	LD DISPLAYCUATRO_FECHA, X

;modo donde se muestra la hora
MODO_0:
	CPI MODO, 0 ;Comparamos para ver si estamos en el modo correcto.
	BRNE MODO_1	;si no lo mandamos a comparar a al siguente modo
	//----500ms----// 
	SBRS CONT_TIMER0, 0 ;Analizamos si cada el bit 0 de CONT_TIMER0 esta en 1 para encender la led de los segundos =
	SBI PORTC, 0
	SBRC CONT_TIMER0, 0 ; luego miramos si esta en 0 para apagarla 
	CBI PORTC, 0
	CBI PORTC, 5 ;apagamos en pin 5 de PORTC por si estaba encendido al salir del modo 5 
	CBI PORTB, 5
//----indicador de hora----//
	SBI PORTB, 3 ;encendemos el indicador de modo hora 
//----Inicio de la multiplexacion----//
	CALL MULTIPLEXACION ;se manda a llamar la subrutina que muestra la hora en los displays
	RJMP LOOP

MODO_1:
;se analiza se esta en el modo 1 que es mostrar la fecha
	CPI MODO, 1
	BRNE MODO_2	; si no esta en este modo se va a modo 2 
	CBI PORTB, 3	;se apaga el indicador de fecha
	SBI PORTB, 4; se enciende el indicador de fecha 
	SBI PORTC, 0; se pausan las luces de los segundos en el display y se dejan encendidas 
//----MULTIPLEXACION FECHA----//
	CALL MULTIPLEXACION_FECHA	;se llama a la subrutina que permite mostrar a fecha 
	RJMP LOOP 

MODO_2:
	CPI MODO, 2 ;se compara si estamos en el modo 2 que es el de configurar hora
	BRNE MODO_3 ; si no es asi manda al siguiente modo
	CBI PORTB, 4	;se apaga el indicador de fecha
//----INDICADOR MODO CONFIG HORA----//
	SBRS CONT_TIMER0, 0 ;Analizamos si cada 
	SBI PORTB, 3
	SBRC CONT_TIMER0, 0
	CBI PORTB, 3
	CBI PORTC, 0
//---cambio de hora manual---//
	CALL DECENAS_UNIDADES_MIN	;se manda a llamar a la subrutina que permite hacer el incremento manual de los minutos
	CALL UNIDADES_DECENAS_HOR	;se manda a llamar a la subrutina que permite hacer el incremento manual de las horas 
	CALL MULTIPLEXACION		;se llama a la subrutina que permite mostrar la hora manual 
	RJMP LOOP
MODO_3:
	CPI MODO, 3 ;se analiza si esta en el modo de configuracion de fecha
	BRNE MODO_4	;si no salta al sigguiente modo
	CBI PORTB, 3 ;desactiva el indicador de configuracion de hora
//----INDICADOR CONFIG FECHA----//
	SBRS CONT_TIMER0, 0 ;encendemos cada 500ms la led de indicador de fecha 
	SBI PORTB, 4
	SBRC CONT_TIMER0, 0
	CBI PORTB, 4
//----CONFIGURACION FECHA----//
	CALL FECHA_MANUAL	;rutina que permite aumentar manualmente pero solo para los dias 
	CALL FECHA_MESES_MANUAL	;rutina que permite incrementar manualmente los meses 
	CALL MULTIPLEXACION_FECHA	;permite mostra la configuracion en los displays 
	RJMP LOOP 

MODO_4:
	CPI MODO, 4 ;analiza si estamos seleccionano el modo de la configuracion de la alarma
	BRNE MODO_5	;si no salta al siguiente
	CBI PORTB, 4
//----Indicador de CONFIG ALARMA----//
	SBRS CONT_TIMER0, 0 ;encedemos en indicador cada 500ms 
	SBI PORTB, 5	
	SBRC CONT_TIMER0, 0
	CBI PORTB, 5
//CONFIGURACION ALARMA MANUAL----//
	CALL CONFIG_ALARMA_MINUTOS	;llamamos a la rutina que permite configurar los minutos manualmente
	CALL CONFIG_ALARMA_HORAS	;llamamos a la rutina que permite configurar las horas manualmente
	CALL MULTIPLEXACION_ALARMA	;llamamos a la funcion que permite mostrar la alarma deseada en los displays
	RJMP LOOP
	 
MODO_5:
	CPI MODO, 5 ;comparamos si estamos en el modo 5 que es el modo que arma la alarma. 
	BRNE FIN
	SBRS CONT_TIMER0, 0 ;este es el indicador de segundos 
	SBI PORTC, 0
	SBRC CONT_TIMER0, 0
	CBI PORTC, 0
//----INDICADOR DE ALARMA ARMADA----//
	SBI PORTB, 5
	SBI PORTB, 3
	CALL MULTIPLEXACION ;aqui se muestra la hora
//----LOGICA DE COMPARACION DE ALARMA----//
;se compara si todos los registros de la hora son iguales a los de configuracion alarma.
	CP UNIDADES_MINUTOS, ALARMA_MINUTOS_UNIDADES
	BREQ BANDERA_1
	RJMP LOOP
BANDERA_1:
	CP DECENAS_MINUTOS, ALARMA_MINUTOS_DECENAS
	BREQ BANDERA_2 
	RJMP LOOP 
BANDERA_2:
	CP UNIDADES_HORAS, ALARMA_HORAS_UNIDADES
	BREQ BANDERA_3
	RJMP LOOP 
BANDERA_3:
	CP DECENAS_HORAS, ALARMA_HORAS_DECENAS
	BREQ BANDERA_4
	RJMP LOOP
BANDERA_4:
	SBI PORTC, 5 ;si son iguales suena la alarma.
	RJMP LOOP
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
	INC MODO ;incrementa el modo 
BIT_1:
;esta logica lo que permite es poder hacer incrementos dependiendo de los modos en los que se encuentra el reloj	
	SBRC R16, 1 
	RJMP BIT_2
	CPI MODO, 2
	BREQ INCREMENTO_HORA_MINUTOS
	CPI MODO, 3
	BREQ INCREMENTO_FECHA_DIAS
	CPI MODO, 4
	BREQ INCREMENTO_ALARMA_MINUTOS
	RJMP BIT_2
INCREMENTO_HORA_MINUTOS:
	INC UNIDADES_MINUTOS ;solo permite hacer incrementos en los minutos si esta en el modo 2
	RJMP BIT_2
INCREMENTO_FECHA_DIAS:
	INC UNIDADES_DIAS ; solo permite hacer incrementos en los dias si esta en el modo 3
	RJMP BIT_2
INCREMENTO_ALARMA_MINUTOS:
	INC ALARMA_MINUTOS_UNIDADES ;solo permite hacer incrementos si se encuentra en el modo 4
BIT_2:
	SBRC R16, 2
	RJMP COMPARACION
	CPI MODO, 2
	BREQ INCREMENTO_HORAS
	CPI MODO, 3
	BREQ INCREMENTO_FECHA_MESES
	CPI MODO, 4
	BREQ INCREMENTO_ALARMA_HORAS
	RJMP COMPARACION
INCREMENTO_HORAS:
	INC UNIDADES_HORAS ;solo permite hacer incrementos en las horas si esta en el modo 2
	RJMP COMPARACION
INCREMENTO_FECHA_MESES:
	INC UNIDADES_MES ;solo permite hacer incrementos en los mses si esta en el modo 3
	RJMP COMPARACION
INCREMENTO_ALARMA_HORAS:
	INC ALARMA_HORAS_UNIDADES ; solo permite hacer incrementos en las horas de la alarma si solo esta en el modo 4
COMPARACION:
	CPI MODO, 6 ;si el contador se paso de 5 lo limpia y regresa a 0/
	BRNE EXIT 
	CLR MODO
EXIT:
	RETI
//----SUBRUTINA DE MULTIPLEXACION
MULTIPLEXACION:
;la rutina de multiplexacion funciona de forma tal que cada 5 ms aumenta 1 unidad, y cada vez que incrementa 1 unidad corresponde a un display 
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
;si existe un incremento de los minutos en la interrupcion este aumente de posicion en la tabla 
DECENAS_UNIDADES_MIN:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CPI UNIDADES_MINUTOS, 10	;si se pasa de nueve el contador hace overflow 
	BRSH DECENAS_MIN_MANUAL
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	RET
DECENAS_MIN_MANUAL:
;esta es la logica para que el display de unidades de minutos haga overflow 
`	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR	UNIDADES_MINUTOS
	ADD XL, UNIDADES_MINUTOS
	LD DISPLAYUNO, X
	INC DECENAS_MINUTOS ;incrementa cada vez que el el display de unidades de minutos se pase de 9 
	CPI DECENAS_MINUTOS, 6 
	BRSH OVERFLOW_UNIDADES_MIN 
	CLR R26 
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYDOS, X; se saca el valor de la decenas de minutos
	RET
OVERFLOW_UNIDADES_MIN:
;logica que reinicia las unidades de minutos y decenas de minutos
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR DECENAS_MINUTOS
	ADD XL, DECENAS_MINUTOS	
	LD DISPLAYDOS, X
	RET

//----RUTINA DECENAS UNIDADES HORAS----//
;si existe un incremento de las horas  en la interrupcion este aumente de posicion en la tabla
UNIDADES_DECENAS_HOR:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CPI UNIDADES_HORAS, 4 ;compara si es cuatro, ya que necesita estar chequeando constante mente si ya se paso de las 23 horas 
	BREQ CASO_ESPECIAL_MANUAL
REGRESO_MANUAL:
	CPI UNIDADES_HORAS, 10 ;si el caso de que se paso de 23 horas no aplica chequea overflow normal en el display 
	BREQ DECENAS_HOR_MANUAL 
	ADD XL, UNIDADES_HORAS
	LD DISPLAYTRES, X
	RJMP FIN_CAMBIOS   
CASO_ESPECIAL_MANUAL:
	CPI DECENAS_HORAS, 2 ;analiza si ya se paso de 23 horas y si si reinicia todos los displays 
	BREQ FIN_RELOJ_MANUAL
	RJMP REGRESO_MANUAL 
DECENAS_HOR_MANUAL:
;logica que incrementa las decenas de horas cada vez que las unidades de horas hacen overflow 
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR UNIDADES_HORAS
	ADD XL, DECENAS_MINUTOS
	LD DISPLAYTRES, X 
	INC DECENAS_HORAS
	CLR R26
	ADD XL, DECENAS_HORAS
	LD DISPLAYCUATRO, X
	RJMP FIN_CAMBIOS
FIN_RELOJ_MANUAL:
;si se pasa de 24 horas manda a reiniciar todos los displays
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
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

//----RUTINA DE MULTIPLEXACION PARA LA FECHA----//
;misma logica que todos los displays, permite encender los displays cada 5ms ya que un contador incrementa cada 5ms
MULTIPLEXACION_FECHA:
	CPI CONT_TIMER2, 0
	BRNE DISPLAY2_FECHA 
	SBI PORTC, 1
	OUT PORTD, DISPLAYUNO_FECHA 
	CBI PORTC, 1
DISPLAY2_FECHA: 
	CPI CONT_TIMER2, 1
	BRNE DISPLAY3_FECHA 
	SBI PORTC, 2
	OUT PORTD, DISPLAYDOS_FECHA
	CBI PORTC, 2
DISPLAY3_FECHA:
	CPI CONT_TIMER2, 2
	BRNE DISPLAY4_FECHA 
	SBI PORTC, 4
	OUT PORTD, DISPLAYTRES_FECHA
	CBI PORTC, 4 
DISPLAY4_FECHA:
	CPI CONT_TIMER2, 3
	BRNE FIN_MULTI_FECHA 
	SBI PORTC, 3
	OUT PORTD, DISPLAYCUATRO_FECHA 
	CBI PORTC, 3 
FIN_MULTI_FECHA:
	RET	

//----SUBRUTINA DE FECHA MANUAL DIAS----//
FECHA_MANUAL:
;cada vez que existe un incremento dentro de la interrupcion este incrementa un dia manual mente
	LDI XH, 0X01
    LDI XL, 0X00
	LD DISPLAYUNO_FECHA, X
    MOV R20, UNIDADES_DIAS
    CPI R20, 9	;realiza el mismo chequeo que permite ver si ya se paso de 29 
    BREQ CASO_FEBRERO_MANUAL ;manda a rutina que permite ver si esta en el mes de febrero
	RJMP REGRESO_NO_FEBRERO_MANUAL;Si no sigue el flujo normal.

CASO_FEBRERO_MANUAL:
;aqui analiza si esta en el caso de febrero y que no se pase de los 28 dias 
    MOV R20, DECENAS_DIAS
    CPI R20, 2
    BRNE REGRESO_NO_FEBRERO_MANUAL
	MOV R20, UNIDADES_MES 
	CPI R20, 2
	BRNE REGRESO_NO_FEBRERO_MANUAL
	MOV R20, DECENAS_MES 
	CPI R20, 1
	BRSH REGRESO_NO_FEBRERO_MANUAL
	RJMP OVERFLOW_DIAS

REGRESO_NO_FEBRERO_MANUAL:
    ; Para meses de 30 días (abril, junio, septiembre, noviembre) 
    MOV R20, UNIDADES_DIAS
    CPI R20, 1 
    BRNE CASO_NO_TREINTA_MANUAL
    MOV R20, DECENAS_DIAS
    CPI R20, 3
    BRNE CASO_NO_TREINTA_MANUAL
    MOV R20, UNIDADES_MES
    CPI R20, 4
    BRNE SEGUIR_MANUAL    ; Abril
	RJMP OVERFLOW_DIAS
SEGUIR_MANUAL:
    CPI R20, 6
    BRNE SEGUIR_MANUAL2    ; Junio
	RJMP OVERFLOW_DIAS
SEGUIR_MANUAL2:
    CPI R20, 9
    BRNE SEGUIR_MANUAL3
	RJMP OVERFLOW_DIAS 
SEGUIR_MANUAL3:
	MOV R20, UNIDADES_MES
	CPI R20, 1
	BRNE CASO_NO_TREINTA_MANUAL
    MOV R20, DECENAS_MES
    CPI R20, 1
    BRNE CASO_NO_TREINTA_MANUAL    ; Noviembre (mes 11)
	RJMP OVERFLOW_DIAS 

CASO_NO_TREINTA_MANUAL:
    ; Para meses de 31 días, el overflow se produce al llegar al día 32
    MOV R20, UNIDADES_DIAS
    CPI R20, 2
    BRNE FLUJO_NORMAL_MANUAL
    MOV R20, DECENAS_DIAS
    CPI R20, 3
    BREQ OVERFLOW_DIAS
	RJMP FLUJO_NORMAL_MANUAL
OVERFLOW_DIAS:
	CLR UNIDADES_DIAS
	INC UNIDADES_DIAS 
	CLR DECENAS_DIAS
	LDI XL, 0X01
	LD DISPLAYUNO_FECHA, X
	LDI XL, 0X00
	LD DISPLAYDOS_FECHA, X
	RET

FLUJO_NORMAL_MANUAL:
;si no existe overflow permite seguir con el flujo normal para poder seguir incrementando 
    MOV R20, UNIDADES_DIAS
    CPI R20, 10
    BRSH DECENAS_DIA_MANUAL
    ADD XL, UNIDADES_DIAS
    LD DISPLAYUNO_FECHA, X
    RET

DECENAS_DIA_MANUAL:
;si existe overflow entre aqui para aumentar las decenas de dia manual mente 
    CLR R26
    LDI XH, 0X01
    LDI XL, 0X00
    CLR UNIDADES_DIAS
    INC DECENAS_DIAS
    LD DISPLAYUNO_FECHA, X
    ADD XL, DECENAS_DIAS
    LD DISPLAYDOS_FECHA, X
    RET

//----RUTINA MESES CONFIG MANUAL----//
;se aplica la misma logica que arriba solo que en lugar de incrementarse automaticamente se incrementa manual mente 
FECHA_MESES_MANUAL:
    LDI XH, 0X01
    LDI XL, 0X00
    MOV R20, UNIDADES_MES
    CPI R20, 3	;aqui analiza si no se ha pasado de 12 meses y reinicia automaticamente 
    BREQ CASO_ESPECIAL_FECHA_MANUAL

REGRESO_FECHA_MANUAL:
    CPI R20, 10 ;si no se ha pasado de los 12 meses sigue el flujo manual de los dias 
    BRSH DECENAS_MESES_MANUAL
    ADD XL, UNIDADES_MES
    LD DISPLAYTRES_FECHA, X
	RET
	  
CASO_ESPECIAL_FECHA_MANUAL:
    MOV R20, DECENAS_MES
    CPI R20, 1	;si ya se pasaron los 12 meses lo manda al overflow 
    BREQ FIN_FECHA_MANUAL
    RJMP REGRESO_FECHA_MANUAL

DECENAS_MESES_MANUAL:
;cada vez que el display de unidades de meses hace overflow incrementa en esta logica. 
    LDI XH, 0X01
    LDI XL, 0X00
	CLR UNIDADES_MES
    INC DECENAS_MES
    LD DISPLAYTRES_FECHA, X
    ADD XL, DECENAS_MES
    LD DISPLAYCUATRO_FECHA, X
    RET

FIN_FECHA_MANUAL:
;si se paso de los 12 meses regresa a todo los displays a 01/01
    LDI XH, 0X01
    LDI XL, 0X00
	CLR UNIDADES_DIAS
    INC UNIDADES_DIAS 
	CLR DECENAS_DIAS 
	CLR UNIDADES_MES 
	INC UNIDADES_MES 
	CLR DECENAS_MES
    LDI XL, 0X01
	LD DISPLAYUNO_FECHA, X
	LDI XL, 0X00
	LD DISPLAYDOS_FECHA, X
	LDI XL, 0X01
	LD DISPLAYTRES_FECHA, X
	LDI XL, 0X00
	LD DISPLAYCUATRO_FECHA, X
	RET

//----RUTINA DE CONFIGURACION DE ALARMA----//
;este es una copia de  configuracion de hora manual
CONFIG_ALARMA_MINUTOS:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	MOV R20, ALARMA_MINUTOS_UNIDADES
	CPI R20, 10
	BRSH DECENAS_MIN_ALARMA_MANUAL
	ADD XL, ALARMA_MINUTOS_UNIDADES
	LD ALARMA_DISPLAYUNO, X
	RET
DECENAS_MIN_ALARMA_MANUAL:
`	LDI XH, 0X01
	LDI XL, 0X00
	//CLR R26
	CLR	ALARMA_MINUTOS_UNIDADES
	ADD XL, ALARMA_MINUTOS_UNIDADES
	LD ALARMA_DISPLAYUNO, X
	INC ALARMA_MINUTOS_DECENAS
	MOV R20, ALARMA_MINUTOS_DECENAS
	CPI R20, 6 
	BRSH OVERFLOW_ALARMA_MIN 
	CLR R26 
	ADD XL, ALARMA_MINUTOS_DECENAS
	LD ALARMA_DISPLAYDOS, X
	RET
OVERFLOW_ALARMA_MIN:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR ALARMA_MINUTOS_DECENAS
	ADD XL, ALARMA_MINUTOS_DECENAS
	LD ALARMA_DISPLAYDOS, X
	RET
//----RUTINA MULTIPLEXACION ALARMA----//
;se encienden cada 5ms los displays por medio de un contador que incrementa cada 5 ms 
MULTIPLEXACION_ALARMA:
	CPI CONT_TIMER2, 0
	BRNE DISPLAY2_ALARMA  
	SBI PORTC, 1
	OUT PORTD, ALARMA_DISPLAYUNO 
	CBI PORTC, 1
DISPLAY2_ALARMA: 
	CPI CONT_TIMER2, 1
	BRNE DISPLAY3_ALARMA
	SBI PORTC, 2
	OUT PORTD, ALARMA_DISPLAYDOS
	CBI PORTC, 2
DISPLAY3_ALARMA:
	CPI CONT_TIMER2, 2
	BRNE DISPLAY4_ALARMA 
	SBI PORTC, 4
	OUT PORTD, ALARMA_DISPLAYTRES 
	CBI PORTC, 4 
DISPLAY4_ALARMA:
	CPI CONT_TIMER2, 3
	BRNE FIN_MULTI_ALARMA
	SBI PORTC, 3
	OUT PORTD, ALARMA_DISPLAYCUATRO 
	CBI PORTC, 3 
FIN_MULTI_ALARMA:
	RET

//----RUTINA DE INCREMENTO ALARMA HORAS----//
;este codigo es una copia del codigo de configuracion de hora
CONFIG_ALARMA_HORAS:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	MOV R20, ALARMA_HORAS_UNIDADES
	CPI R20, 4
	BREQ CASO_ESPECIAL_MANUAL_ALARMA
REGRESO_MANUAL_ALARMA:
	CPI R20, 10 
	BREQ DECENAS_HOR_MANUAL_ALARMA 
	ADD XL, ALARMA_HORAS_UNIDADES
	LD ALARMA_DISPLAYTRES, X
	RJMP FIN_CAMBIOS_ALARMA  
CASO_ESPECIAL_MANUAL_ALARMA:
	MOV R20, ALARMA_HORAS_DECENAS
	CPI R20, 2
	BREQ FIN_RELOJ_MANUAL_ALARMA
	RJMP REGRESO_MANUAL_ALARMA 
DECENAS_HOR_MANUAL_ALARMA:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR ALARMA_HORAS_UNIDADES
	ADD XL, ALARMA_MINUTOS_DECENAS
	LD ALARMA_DISPLAYTRES, X 
	INC ALARMA_HORAS_DECENAS
	CLR R26
	ADD XL, ALARMA_HORAS_DECENAS
	LD ALARMA_DISPLAYCUATRO, X
	RJMP FIN_CAMBIOS_ALARMA
FIN_RELOJ_MANUAL_ALARMA:
	LDI XH, 0X01
	LDI XL, 0X00
	CLR R26
	CLR ALARMA_MINUTOS_UNIDADES  
	CLR ALARMA_MINUTOS_DECENAS
	CLR ALARMA_HORAS_UNIDADES
	CLR ALARMA_HORAS_DECENAS
	ADD XL, ALARMA_HORAS_DECENAS
	LD ALARMA_DISPLAYCUATRO, X
	ADD XL, ALARMA_HORAS_UNIDADES
	LD ALARMA_DISPLAYUNO, X
	LD ALARMA_DISPLAYTRES, X 
	LD ALARMA_DISPLAYDOS, X
	LD ALARMA_DISPLAYUNO, X
	RJMP FIN_CAMBIOS_ALARMA 
FIN_CAMBIOS_ALARMA:
	RET