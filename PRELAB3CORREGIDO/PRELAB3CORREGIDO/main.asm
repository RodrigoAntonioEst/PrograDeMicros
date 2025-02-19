/*
 * Prelab3.asm
 *
 * Creado: 18-Feb-25 22:50 PM 
 * Autor: Rodrigo Estrada
 */
.include "M328PDEF.inc"           // Incluir definiciones espec�ficas para el ATmega328P
.cseg 
.org 0x0000
    RJMP start                  // Salta al inicio del programa (etiqueta "start")
.org PCI0addr
    RJMP RUTINA_INTERRUPCION    // Vector de interrupci�n de cambio de pin (Pin Change)


// Configuraci�n de la pila
START:
    LDI R16, LOW(RAMEND)        // Cargar el byte bajo de RAMEND en R16
    OUT SPL, R16                // Inicializar el registro de pila bajo (SPL)
    LDI R16, HIGH(RAMEND)       // Cargar el byte alto de RAMEND en R16
    OUT SPH, R16                // Inicializar el registro de pila alto (SPH)


// Configuraci�n del micro
SETUP:
    CLI                         // Deshabilitar interrupciones globales

    // Configuraci�n de entradas y salidas
    LDI R16, 0X00               // Cargar 0x00 en R16 para configurar entradas
    OUT DDRB, R16               // Configurar PORTB como entrada
    LDI R16, 0XFF               // Cargar 0xFF en R16 para activar los pull-ups
    OUT PORTB, R16              // Activar los pull-ups en PORTB

    LDI R16, 0XFF               // Cargar 0xFF en R16 para configurar salidas
    OUT DDRC, R16               // Configurar PORTC como salida
    LDI R16, 0X00               // Cargar 0x00 en R16 para inicializar las salidas
    OUT PORTC, R16              // Apagar todos los LEDs conectados a PORTC

    // Configuraci�n de la interrupci�n de Pin Change en PB0 y PB1
    LDI R16, (1 << PCINT0) | (1 << PCINT1)  // Configurar la m�scara para los pines PB0 y PB1
    STS PCMSK0, R16             // Guardar la m�scara en el registro PCMSK0
    LDI R16, (1 << PCIE0)       // Habilitar el grupo de interrupciones de Pin Change para PORTB
    STS PCICR, R16              // Guardar en el registro PCICR

    LDI R17, 0X00               // Inicializar el contador en R17 a 0

    SEI                         // Habilitar interrupciones globales

// Loop principal (vac�o, ya que se usan interrupciones)
LOOP:
    RJMP LOOP                 // Bucle infinito


// Rutina de interrupci�n para Pin Change
RUTINA_INTERRUPCION:
    PUSH R16                  // Guardar R16 en la pila
    IN   R16, SREG            // Leer el registro de estado (SREG) en R16
    PUSH R16                  // Guardar SREG en la pila

    IN   R16, PINB            // Leer el estado actual de PORTB en R16

    // Verifica si PB0 est� en bajo (bot�n presionado) para incrementar
    SBRS R16, 0               // Saltar la siguiente instrucci�n si el bit 0 de R16 est� en 1 (bot�n no presionado)
    RJMP NOINCREMENTAR        // Si el bit 0 es 0 (bot�n presionado), no salta y contin�a con la instrucci�n siguiente
    INC  R17                 // Incrementar el contador en R17
    CPI R17, 0x10            // Comparar R17 con 0x10
    BRNE NOINCREMENTAR        // Si R17 no es igual a 0x10, salta a la etiqueta NOINCREMENTAR
    LDI R17, 0X00            // Si R17 es igual a 0x10, reiniciar el contador a 0

NOINCREMENTAR:
    // Verifica si PB1 est� en bajo (bot�n presionado) para decrementar
    SBRS R16, 1               // Saltar la siguiente instrucci�n si el bit 1 de R16 est� en 1 (bot�n no presionado)
    RJMP NODECREMENTAR        // Si el bit 1 es 0 (bot�n presionado), no salta y contin�a con la instrucci�n siguiente
    DEC  R17                 // Decrementar el contador en R17
    CPI R17, 0XFF            // Comparar R17 con 0xFF
    BRNE NODECREMENTAR        // Si R17 no es igual a 0xFF, salta a la etiqueta NODECREMENTAR
    LDI R17, 0X0F            // Si R17 es igual a 0xFF, cargar 0x0F en R17

NODECREMENTAR:
    OUT PORTC, R17           // Mostrar el valor del contador en PORTC (LEDs)

    POP  R16                 // Recuperar el valor previamente guardado de SREG (almacenado en R16)
    OUT SREG, R16            // Restaurar el registro SREG
    POP  R16                 // Recuperar el valor original de R16
    RETI                     // Retornar de la interrupci�n
