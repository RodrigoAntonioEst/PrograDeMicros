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
	RJMP RUTINA_TIMER0 


// Configuración de la pila
START:
    LDI R16, LOW(RAMEND)        // Cargar el byte bajo de RAMEND en R16
    OUT SPL, R16                // Inicializar el registro de pila bajo (SPL)
    LDI R16, HIGH(RAMEND)       // Cargar el byte alto de RAMEND en R16
    OUT SPH, R16                // Inicializar el registro de pila alto (SPH)
	
	// Definimos un valor de 17 para SIZE (16 bytes + 1 para fin de tabla)
	.equ SIZE = 17 // Esto se hace para poder finalizar el traslado de flash ya que contamos con 16 bytes 

	// Tabla de 16 bytes (MYTABLE) que serán copiados a SRAM
	MYTABLE: .DB 0B01000000, 0B01111001, 0B00100100, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
	LDI ZH, HIGH(MYTABLE << 1)           // Cargar en ZH la parte alta de la dirección de MYTABLE (para LPM)
	LDI ZL, LOW(MYTABLE << 1)            // Cargar en ZL la parte baja de la dirección de MYTABLE (para LPM)
	LDI XH, 0X02                          // Cargar en XH el valor 0x02 (destino en SRAM)
	LDI XL, 0X01                          // Cargar en XL el valor 0x01 (destino en SRAM)
	LDI R21, SIZE                         // Guardar en R21 el tamaño de la tabla (17)

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
    LDI R16, 0XFF               // Cargar 0xFF en R16 para activar los pull-ups
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
    LDI XL, 0X11                     // XL = 0x11
    LDI XH, 0X02                     // XH = 0x02 => dirección en SRAM
    ST X, R18                        // Guardar 0xFF en SRAM[X=0x0211]

	// Apuntar a la dirección donde se encuentran nuestros datos transferidos en X
    LDI XH, 0X02                     // XH = 0x02
    LDI XL, 0X01                     // XL = 0x01 => apunta a 0x0201
    LD  R20, X                       // Cargar en R20 el byte en SRAM[0x0201]
    OUT PORTD, R20                   // Mostrar en PORTD el valor leído

    SEI                         // Habilitar interrupciones globales

// Loop principal (vacío, ya que se usan interrupciones)
LOOP:
	CPI R31, 0X64
	BRNE LOOP
	CLR R31
	LD R20, X+                       // Leer de X y luego incrementar X
	CPI R20, 0XFF                    // Comparar con 0xFF
    BRNE ATRAS                       // Si no es 0xFF, saltar a ATRAS
    LDI XL, 0X01                     // Reiniciar puntero X a 0x0201
    LDI XH, 0X02
    LD  R20, X                       // Cargar en R20 el valor en SRAM[0x0201]
ATRAS:
	OUT PORTD, R20 
    RJMP LOOP                 // Bucle infinito


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
	
	LDI R16, 0X64
	OUT TCNT0, R16'
	INC R31
	RETI 