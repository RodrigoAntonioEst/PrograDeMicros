/*
 * SLAVE_I2C.c
 *
 * Created: 5/02/2026 19:17:54
 * Author : rodro
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define Slaveadress 0x30

#include "I2C/I2C.h"
#include "ADC/ADC.h"

/****************************************/
// Globals
volatile uint8_t valorADC = 0;   // valor que el master leerá
volatile uint8_t buffer   = 0;   // último byte recibido del master (por si usás comandos)
volatile uint8_t MULTIPLEXADO = 0;

/****************************************/
// Main
int main(void)
{
    // LED debug en PB5 (Arduino Nano: D13)
    DDRB  |= (1<<DDB5);
    PORTB &= ~(1<<PORTB5);

    // ADC: tu librería (asumo: habilita interrupción ADC_vect)
    init_ADC(1, 128, 0);
    // Si tu init_ADC NO arranca la primera conversión, descomentá:
    // ADCSRA |= (1<<ADSC);

    // I2C Slave init
    I2C_slave_init(Slaveadress);

    sei();

    while (1)
    {
        // Debug opcional: si el master te escribe 'R', toggle LED
        if(buffer == 'R'){
            PINB |= (1<<PINB5);
            buffer = 0;
        }
        // Nada más: el slave trabaja por interrupciones
    }
}

/****************************************/
// ISR: TWI (I2C) SLAVE
ISR(TWI_vect)
{
    uint8_t estado = (TWSR & 0xF8);   // ? máscara correcta: status en bits 7..3

    switch(estado)
    {
        // --- MASTER -> SLAVE (SLA+W) ---
        case 0x60: // SLA+W recibido, ACK devuelto
        case 0x70: // General call recibido
            // listo para recibir datos
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        case 0x80: // dato recibido, ACK devuelto
        case 0x90: // dato recibido (general call)
            buffer = TWDR;  // guardo lo que envió el master
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        // --- MASTER <- SLAVE (SLA+R) ---
        case 0xA8: // SLA+R recibido, ACK devuelto
        case 0xB8: // dato transmitido, ACK recibido (piden otro byte)
            TWDR = valorADC; // ? responder ADC (1 byte)
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        // --- Fin de lectura (NACK) / fin de transmisión ---
        case 0xC0: // dato transmitido, NACK recibido (ya no quieren más)
        case 0xC8: // último dato transmitido, ACK recibido
            // volver a modo “listo/escucha”
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        // --- STOP o Repeated START detectado ---
        case 0xA0:
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        default:
            // recuperación segura
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;
    }
}

/****************************************/
// ISR: ADC
ISR(ADC_vect)
{
    // Tu lógica original (dejada igual) — 8 bits en ADCH
    switch(MULTIPLEXADO){
        case 1:
            valorADC = ADCH;   // ? este es el que el master va a leer
            pinADC(0, 1);      // según tu librería (ojo: revisa si es el canal correcto)
            break;

        case 2:
            // si no usás otro canal, podés dejar vacío
            break;

        default:
            MULTIPLEXADO = 0;
            pinADC(1, 1);
            break;
    }

    MULTIPLEXADO++;
    ADCSRA |= (1<<ADSC); // siguiente conversión
}
