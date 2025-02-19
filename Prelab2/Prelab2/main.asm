/*
 * Prelab2.asm
 *
 * Creado: 11-Feb-25 11:10 AM
 * Autor: Rodrigo Estrada
 */

//cargamos los datos a la flash
.include "M328PDEF.inc"              // Incluir definiciones específicas para el ATmega328P
.cseg                                 // Sección de código
.org 0x0000                           // Dirección inicial (vector de reset)

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
LDI ZH, HIGH(MYTABLE << 1)           // Restablecer ZH a la dirección de MYTABLE
LDI ZL, LOW(MYTABLE << 1)            // Restablecer ZL a la dirección de MYTABLE
LDI YH, 0X01                          // Cargar en YH el valor 0x01 (otra posición en SRAM)
LDI YL, 0X01                          // Cargar en YL el valor 0x01 (otra posición en SRAM)
LDI R21, SIZE                         // Volver a cargar el tamaño (17) en R21

// Etiqueta COPIARY: Copia desde Flash (Z) hacia SRAM (Y)
COPIARY:
    LPM R24, Z+                       // Cargar en R24 el byte apuntado por Z, e incrementar Z
    DEC R21                           // Decrementar R21
    CPI R21, 0                        // Comparar con 0
    BREQ DONE2                        // Si R21 == 0, saltar a DONE2
    ST Y+, R24                        // Guardar en SRAM (Y) el contenido de R24 y luego incrementar Y
    RJMP COPIARY                      // Repetir hasta terminar

// Etiqueta DONE2: fin de la segunda copia
DONE2:

// Configuración de la pila
LDI R16, LOW(RAMEND)                 // Cargar en R16 el byte bajo de la última posición de RAM
OUT SPL, R16                         // Inicializar el puntero de pila bajo (SPL)
LDI R16, HIGH(RAMEND)                // Cargar en R16 el byte alto de la última posición de RAM
OUT SPH, R16                         // Inicializar el puntero de pila alto (SPH)

// Configuración del Micro
SETUP:
    // Bajar la frecuencia del procesador (CLKPR)
    LDI R16, (1 << CLKPCE)           // Cargar en R16 el bit para habilitar cambios en CLKPR
    STS CLKPR, R16                   // Guardar en CLKPR (habilitar cambios)
    LDI R16, 0x08                    // Cargar en R16 el valor 0x08 (baja frecuencia a 62.5 KHz si CKDIV8)
    STS CLKPR, R16                   // Guardar en CLKPR el nuevo valor

    // Inicializar Timer0
    LDI R16, (1 << CS01 | 1 << CS00) // Prescaler de 1024 (CS02:0 = 0b011)
    OUT TCCR0B, R16                  // Configurar TCCR0B con prescaler 1024
    LDI R16, 0x9E                    // Valor inicial para TCNT0 (para desbordar a ~100 ms)
    OUT TCNT0, R16                   // Cargar TCNT0 con 0x9E

    // Configurar PORTC como salida
    LDI R16, 0XFF                    // 0xFF para salidas
    OUT DDRC, R16                    // DDRC = 0xFF => todo PORTC en salida
    LDI R16, 0X00                    // Valor inicial 0
    OUT PORTC, R16                   // Apagar los LEDs en PORTC

    // Configurar PORTD como salida
    LDI R16, 0XFF                    // 0xFF para salidas
    OUT DDRD, R16                    // DDRD = 0xFF => todo PORTD en salida
    LDI R16, 0X00                    // Valor inicial 0
    OUT PORTD, R16                   // Apagar todos los LEDs en PORTD

    // Configurar PORTB como entrada
    LDI R16, 0b00000100              // Solo PB2 como salida, PB0..PB1 y PB3..PB5 como entrada (o viceversa)
    OUT DDRB, R16                    // DDRB = 0b00000100
    LDI R16, 0b11111011              // Activar pull-ups excepto PB2
    OUT PORTB, R16                   // PORTB = 0b11111011

    // Guardar datos para antirrebote
    LDI R18, 0X01                    // Valor a almacenar
    LDI XL, 0X00                     // XL = 0x00
    LDI XH, 0X02                     // XH = 0x02 => dirección en SRAM
    ST X, R18                        // Guardar 0x01 en SRAM[X=0x0200]

    LDI R18, 0XFF                    // Valor 0xFF
    LDI XL, 0X11                     // XL = 0x11
    LDI XH, 0X02                     // XH = 0x02 => dirección en SRAM
    ST X, R18                        // Guardar 0xFF en SRAM[X=0x0211]

    // Cargar un valor para overflow de Y
    LDI R18, 0XFF                    // Valor 0xFF
    LDI YL, 0X11                     // YL = 0x11
    LDI YH, 0X01                     // YH = 0x01 => dirección en SRAM
    ST Y, R18                        // Guardar 0xFF en SRAM[Y=0x0111]

    // Apuntar a la dirección donde se encuentran nuestros datos transferidos en X
    LDI XH, 0X02                     // XH = 0x02
    LDI XL, 0X01                     // XL = 0x01 => apunta a 0x0201
    LD  R20, X                       // Cargar en R20 el byte en SRAM[0x0201]
    OUT PORTD, R20                   // Mostrar en PORTD el valor leído

    // Apuntar a la dirección donde transferimos con Y
    LDI YL, 0X01                     // YL = 0x01
    LDI YH, 0X01                     // YH = 0x01 => apunta a 0x0101
    LD R24, Y                        // Cargar en R24 el byte en SRAM[0x0101]

    LDI R17, 0XFF                    // Guardar en R17 el estado de los botones (valor inicial)
    LDI R18, 0x00                    // R18 = 0x00 => contador de bits para LEDs
    LDI R30, 0X00                    // R30 = 0 => se usará en subrutinas
    LDI R29, 0X00                    // R29 = 0 => se usará en subrutinas

// Programa principal
LOOP:
    CALL CONTADOR_100MS              // Llamar a la subrutina que cuenta ~100 ms
    CALL COMPARADOR                  // Llamar a la subrutina de comparación
    CALL DISPLAY7SEG                 // Llamar a la subrutina de display 7 segmentos
    RJMP LOOP                        // Repetir infinitamente

// SUBRUTINA DE COMPARACION
COMPARADOR:
    CP R24, R20                      // Comparar R24 con R20
    BREQ RESET                       // Si son iguales, saltar a RESET
    RET                              // Si no son iguales, regresar

RESET:
    LDI YL, 0X01                     // Resetear el puntero Y a 0x0101
    LDI YH, 0X01
    LD  R24, Y                       // Cargar en R24 el valor en SRAM[0x0101]
    CLR R18                          // Limpiar R18 (contador)
    OUT PORTC, R18                   // Apagar LEDs en PORTC
    SBI PINB, 2                      // Toggle bit 2 de PINB (puede ser para depuración)
    RET                              // Regresar

// SUBRUTINA DE DISPLAY DE 7 SEGMENTOS
DISPLAY7SEG:
    IN R16, PINB                     // Leer el estado de PORTB en R16
    CP R16, R17                      // Comparar con R17 (estado anterior)
    BRNE SALTO                       // Si son distintos, saltar a SALTO
    RET                              // Si son iguales, no hay cambio, regresar

SALTO:
    CALL DELAY                       // Llamar a la subrutina DELAY (antirrebote)
    IN R16, PINB                     // Leer de nuevo el estado de PORTB
    CP R16, R17                      // Comparar con el estado anterior
    BRNE SALTO2                      // Si sigue siendo distinto, saltar a SALTO2
    RET                              // Si ahora es igual, no hacer nada más

SALTO2:
    MOV R17, R16                     // Actualizar R17 con el nuevo estado
    SBRC R17, 0                      // "Skip next if bit 0 is cleared" => si bit 0=1, salta la siguiente
    RJMP ATRAS                       // Si bit 0=0, salta a ATRAS
    LD R20, X+                       // Leer de X y luego incrementar X
    CPI R20, 0XFF                    // Comparar con 0xFF
    BRNE ATRAS                       // Si no es 0xFF, saltar a ATRAS
    LDI XL, 0X01                     // Reiniciar puntero X a 0x0201
    LDI XH, 0X02
    LD  R20, X                       // Cargar en R20 el valor en SRAM[0x0201]

ATRAS:
    SBRC R17, 1                      // "Skip next if bit 1 is cleared" => si bit 1=1, salta
    RJMP FIN                         // Si bit 1=0, salta a FIN
    LD R20, -X                       // Leer de X y decrementar X
    CPI R20, 0X01                    // Comparar con 0x01
    BRNE FIN                         // Si no es 0x01, saltar a FIN
    LDI XL, 0X10                     // Reiniciar puntero X a 0x0210
    LDI XH, 0X02
    LD R20, X                        // Cargar en R20 el valor en SRAM[0x0210]

FIN:
    OUT PORTD, R20                   // Mostrar en PORTD el valor en R20
    RET                              // Regresar

// SUBRUTINA DEL CONTADOR CADA 100ms
CONTADOR_100MS:
    IN R16, TIFR0                    // Leer TIFR0 (Registro de banderas de interrupción del Timer0)
    SBRS R16, TOV0                   // "Skip next if bit TOV0 is set" => si TOV0=1, salta
    RET                              // Si TOV0=0, regresar (no ha llegado al overflow)
    SBI TIFR0, TOV0                  // Limpiar la bandera TOV0 (escribiendo 1)
    LDI R16, 0x9E                    // Recargar TCNT0 con 0x9E
    OUT TCNT0, R16                   // Actualizar TCNT0
    INC R30                          // Incrementar R30
    CPI R30, 0X0A                    // Comparar R30 con 0x0A
    BRNE FINCONTADOR                 // Si no es 0x0A, saltar a FINCONTADOR
    CLR R30                          // Si es 0x0A, reiniciar R30
    INC R18                          // Incrementar R18 (contador de 100ms)
    LD R24, Y+                       // Leer de Y y post-incrementar
    CPI R24, 0XFF                    // Comparar con 0xFF
    BRNE NOTOVERFLOW                 // Si no es 0xFF, saltar
    LDI YL, 0X01                     // Reiniciar puntero Y a 0x0101
    LDI YH, 0X01
    LD R24, X                        // Cargar en R24 el valor en X (no muy claro, pero parte de la lógica)

NOTOVERFLOW:
    CPI R18, 0X10                    // Comparar R18 con 0x10 (16 en decimal)
    BRBS 1, OVERFLOW                 // Si el bit 0 del SREG (carry) no se usa aquí, quizás confuso, pero se asume salto
    OUT PORTC, R18                   // Mostrar R18 en PORTC
    RET                              // Regresar

OVERFLOW:
    LDI R18, 0X00                    // Resetear R18 a 0
    OUT PORTC, R18                   // Apagar LEDs en PORTC

FINCONTADOR:
    RET                              // Regresar

// SUBRUTINA DE INTERRUPCION / DELAY (para evitar rebotes)
DELAY:
    LDI R25, 0                       // R25 = 0
SUBDELAY1:
    INC R25                          // Incrementar R25
    CPI R25, 0                       // Comparar con 0
    BRNE SUBDELAY1                   // Repetir hasta que R25 dé la vuelta y sea 0
    LDI R25, 0                       // Reiniciar R25

SUBDELAY2:
    INC R25                          // Incrementar R25
    CPI R25, 0
    BRNE SUBDELAY2
    LDI R25, 0

SUBDELAY3:
    INC R25                          // Incrementar R25
    CPI R25, 0
    BRNE SUBDELAY3
    RET                              // Terminar el delay
