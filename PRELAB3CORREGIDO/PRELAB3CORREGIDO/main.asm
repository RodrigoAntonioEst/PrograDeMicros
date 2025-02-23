/*
 * Prelab3.asm
 *
 * Creado: 18-Feb-25 22:50 PM 
 * Autor: Rodrigo Estrada
 */
.include "M328PDEF.inc"           // Incluir definiciones específicas para el ATmega328P
.cseg 
.org 0x0000
    RJMP start                  // Salta al inicio del programa (etiqueta "start")
.org PCI0addr
    RJMP RUTINA_INTERRUPCION    // Vector de interrupción de cambio de pin (Pin Change)
.org OVF0addr					//VECTOR TIMER0 
	RJMP RUTINA_TIMER0	//establecemos el nombre de la rutina del timer0


// Configuración de la pila
START:
    LDI R16, LOW(RAMEND)        // Cargar el byte bajo de RAMEND en R16
    OUT SPL, R16                // Inicializar el registro de pila bajo (SPL)
    LDI R16, HIGH(RAMEND)       // Cargar el byte alto de RAMEND en R16
    OUT SPH, R16                // Inicializar el registro de pila alto (SPH)
	
	// Definimos un valor de 11 para SIZE (10 bytes + 1 para fin de tabla)
	.equ SIZE = 11 // Esto se hace para poder finalizar el traslado de flash ya que contamos con 10 bytes 

	// Tabla de 16 bytes (MYTABLE) que serán copiados a SRAM
	MYTABLE: .DB 0B01000000, 0B01111001, 0B00100100, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10
	LDI ZH, HIGH(MYTABLE << 1)           // Cargar en ZH la parte alta de la dirección de MYTABLE (para LPM)
	LDI ZL, LOW(MYTABLE << 1)            // Cargar en ZL la parte baja de la dirección de MYTABLE (para LPM)
	LDI XH, 0X02                          // Cargar en XH el valor 0x02 (destino en SRAM)
	LDI XL, 0X01                          // Cargar en XL el valor 0x01 (destino en SRAM)
	LDI R21, SIZE                         // Guardar en R21 el tamaño de la tabla (11)

// Etiqueta COPIAR: Copia desde Flash (Z) hacia SRAM (X) byte a byte
COPIAR:
    LPM R22, Z+                       // LPM carga en R22 el byte de la dirección apuntada por Z, e incrementa Z
    DEC R21                           // Decrementar R21 (contador de bytes restantes)
    CPI R21, 0                        // Comparar R21 con 0
    BREQ DONE                         // Si R21 == 0, saltar a DONE (terminar la copia)
    ST X+, R22                        // Guardar en SRAM (X) el contenido de R22 y luego incrementar X
    RJMP COPIAR                       // Repetir el proceso hasta terminar

// Etiqueta DONE: fin de la primera copia
DONE:
	
	LDI ZH, HIGH(MYTABLE << 1) //cargamos en ZH la parte alta de la direccion de mytable 
	LDI ZL, LOW(MYTABLE << 1) //cargamos en Zl la parte baja de mytable 
	LDI YH, 0X01	//apuntamos a la dirreccion 0x0101 para el registro Y
	LDI YL, 0X01
	LDI R21, SIZE

COPIARY:
	//creamos un bucle que permite copiar en el registro Y los bytes de my table. 
	LPM R20, Z+ //sacamos el valor de Z a r20 luego avanzamos de direccion en flash
	DEC R21	//decrementamos r21
	CPI R21, 0 //si es 0 termina el bucle de escritura
	BREQ DONE2
	ST Y+, R20 //se guarda con post incremento el valor de r20 en el registro Y 
	RJMP COPIARY

DONE2:
// Configuración del micro
SETUP:
    CLI                         // Deshabilitar interrupciones globales

	// Bajar la frecuencia del procesador (CLKPR)
    LDI R16, (1 << CLKPCE)           // Cargar en R16 el bit para habilitar cambios en CLKPR
    STS CLKPR, R16                   // Guardar en CLKPR (habilitar cambios)
    LDI R16, 0x04                    // Cargar en R16 el valor 0x04 (baja frecuencia a 1 MHz )
    STS CLKPR, R16                   // Guardar en CLKPR el nuevo valor

	// Inicializar Timer0
    LDI R16, (1 << CS01 | 1 << CS00) // Prescaler de 64 
    OUT TCCR0B, R16                  // Configurar TCCR0B con prescaler 64
    LDI R16, 0x64                    // Valor inicial para TCNT0 (para desbordar a 10 ms)
    OUT TCNT0, R16                   // Cargar TCNT0 con 0x64



    // Configuración de entradas y salidas
    LDI R16, 0X00               // Cargar 0x00 en R16 para configurar entradas
    OUT DDRB, R16               // Configurar PORTB como entrada
    LDI R16,  0XFF              // Cargar 0xFF en R16 para activar los pull-ups
    OUT PORTB, R16              // Activar los pull-ups en PORTB

    LDI R16, 0XFF               // Cargar 0xFF en R16 para configurar salidas
    OUT DDRC, R16               // Configurar PORTC como salida
    LDI R16, 0X00               // Cargar 0x00 en R16 para inicializar las salidas
    OUT PORTC, R16              // Apagar todos los LEDs conectados a PORTC

	//Configuramos el PORTD como salida 
	LDI R16, 0XFF
	OUT DDRD, R16
	LDI R16, 0X00
	OUT PORTD, R16 

	//configuracion de la interrupcion de timer0
	LDI R16, (1 << TOIE0)
	STS TIMSK0, R16

    // Configuración de la interrupción de Pin Change en PB0 y PB1
    LDI R16, (1 << PCINT0) | (1 << PCINT1)  // Configurar la máscara para los pines PB0 y PB1
    STS PCMSK0, R16             // Guardar la máscara en el registro PCMSK0
    LDI R16, (1 << PCIE0)       // Habilitar el grupo de interrupciones de Pin Change para PORTB
    STS PCICR, R16              // Guardar en el registro PCICR

	

    LDI R17, 0X00               // Inicializar el contador en R17 a 0

	LDI R18, 0XFF                    // Valor 0xFF
    LDI XL, 0X0B                     // XL = 0x11
    LDI XH, 0X02                     // XH = 0x02 => dirección en SRAM
    ST X, R18                        // Guardar 0xFF en SRAM[X=0x0211]

	// Apuntar a la dirección donde se encuentran nuestros datos transferidos en X
    LDI XH, 0X02                     // XH = 0x02
    LDI XL, 0X01                     // XL = 0x01 => apunta a 0x0201
    LD  R20, X                       // Cargar en R20 el byte en SRAM[0x0201]

	LDI YH, 0X01 //apuntamos al inicio de nuestros datos en el registro Y 
	LDI YL, 0X01
	LD R21, Y // lo cargamos a r21 
    SEI                         // Habilitar interrupciones globales

	 

	OUT PORTD, R20 //sacamos el valor de r20 en el portD 
// Loop principal (vacío, ya que se usan interrupciones)
LOOP:
	//creamos la logica para multiplexar los displays de 7 segmentos
	SBRS R31, 0 //el bit 0 de r31 estara en 1 cada 10ms 
	RJMP FIN //si esta en 0 salta a la logica del siguiente display 
	SBI PORTC, 4	//encendemos el port 4 de pinc 
	OUT PORTD, R21	//sacamos la cuent de r21 
	CBI PORTC, 4	//apagamos el pin 4 de portc 

FIN: 
	//logica para le segundo contador 
	SBRC R31, 0 //el bit 0 de r31 estara en 0 cada 10ms 
	RJMP FINAL //si no es el valor deseado termina el bucle y salta a comparar si ya paso 1 segundo 
	SBI PORTC, 5 //encedemos el pin 5 de portc 
	OUT PORTD, R20	//sacamos al portD el valor de r20
	CBI PORTC, 5	//apagamos el pin 5 de portc 
FINAL:
	CPI R31, 0X64	//realizamos la comparacion para saber si ya paso 1 segundo
	BRNE LOOP	//si no ha pasado 1 segundo se regresa al inicio
	CPI R20, 0B01000000	//este es un mecanismo para corregir un error el cual hace que se tarde mas en cambiar de 0 a 1 en el contador de unidades de segundos 
	BRNE EVADIR2 //si no son iguales continua con el flujo normal del codigo 
	LD R20, x+ //si son iguales se desplaza en el registro x
EVADIR2:
	CLR R31 //si se cumplio el segundo se limpia el registro r31
	LD R20, X+                       // Leer de X y luego incrementar X
	CPI R20, 0XFF                    // Comparar con 0xFF
	BRNE ATRAS	//mecanismo para compreneder si existe overflow 
	CPI R21, 0B01000000	//mismo mecanismo para evitar el error al momento de pasar de 0 a 1 en el contador de decenas 
	BRNE EVADIR //si no se cumple salta a evadir 
	LD R21, Y+ //si se cumple se avanza en el registro Y 
EVADIR:
	LDI XL, 0X01                     // Reiniciar puntero X a 0x0201 si existe overflow
    LDI XH, 0X02
	LD  R20, X //se carga el valor de 0 en r20
	LD R21, Y+	//se avanza en Y para el contador de decenas 
	CPI R21, 0x02	//se compara si ya se llego a los 60 segundos
	BRNE ATRAS                       // si no ha llegado se salta a atras 
	LDI YH, 0X01	//si ya se llego a 60 se reinicia a 0 segundos 
	LDI YL, 0X01
	LD R21, Y	//se carga el valor de Y en r21 
ATRAS:
`	//se repite la logica de multiplexacion de los displays 
	SBRS R31, 0
	RJMP FIN2 
	SBI PORTC, 4
	OUT PORTD, R21
	CBI PORTC, 4

FIN2: 
	SBRC R31, 0
	RJMP FINAL2 
	SBI PORTC, 5
	OUT PORTD, R20
	CBI PORTC, 5
FINAL2:
    RJMP	LOOP                 // Bucle infinito


// Rutina de interrupción para Pin Change
RUTINA_INTERRUPCION:
    PUSH R16                  // Guardar R16 en la pila
    IN   R16, SREG            // Leer el registro de estado (SREG) en R16
    PUSH R16                  // Guardar SREG en la pila

    IN   R16, PINB            // Leer el estado actual de PORTB en R16

    // Verifica si PB0 está en bajo (botón presionado) para incrementar
    SBRS R16, 0               // Saltar la siguiente instrucción si el bit 0 de R16 está en 1 (botón no presionado)
    RJMP NOINCREMENTAR        // Si el bit 0 es 0 (botón presionado), no salta y continúa con la instrucción siguiente
    INC  R17                 // Incrementar el contador en R17
    CPI R17, 0x10            // Comparar R17 con 0x10
    BRNE NOINCREMENTAR        // Si R17 no es igual a 0x10, salta a la etiqueta NOINCREMENTAR
    LDI R17, 0X00            // Si R17 es igual a 0x10, reiniciar el contador a 0

NOINCREMENTAR:
    // Verifica si PB1 está en bajo (botón presionado) para decrementar
    SBRS R16, 1               // Saltar la siguiente instrucción si el bit 1 de R16 está en 1 (botón no presionado)
    RJMP NODECREMENTAR        // Si el bit 1 es 0 (botón presionado), no salta y continúa con la instrucción siguiente
    DEC  R17                 // Decrementar el contador en R17
    CPI R17, 0XFF            // Comparar R17 con 0xFF
    BRNE NODECREMENTAR        // Si R17 no es igual a 0xFF, salta a la etiqueta NODECREMENTAR
    LDI R17, 0X0F            // Si R17 es igual a 0xFF, cargar 0x0F en R17

NODECREMENTAR:
    OUT PORTC, R17           // Mostrar el valor del contador en PORTC (LEDs)

    POP  R16                 // Recuperar el valor previamente guardado de SREG (almacenado en R16)
    OUT SREG, R16            // Restaurar el registro SREG
    POP  R16                 // Recuperar el valor original de R16
    RETI                     // Retornar de la interrupción

RUTINA_TIMER0:
	//iniciamos la interrupcion de timer 0
	LDI R16, 0X64	//se carga el valor de donde queremos que inicie a contar para que este cuente 10ms 
	OUT TCNT0, R16 //se carga a tcnt0 para que incie la cuenta 
	INC R31	//se incrementa r31 para despues en el loop comparar si llego a los 100ms 
	RETI	//regresamos de la interrupcion