/*
 * SLAVE2.c
 *
 * Created: 12/02/2026 19:20:50
 * Author : rodro
 */ 
/*
 * SLAVE_I2C.c
 *
 * Created: 5/02/2026 19:17:54
 * Author : rodro
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#define Slaveadress 0x10
#include "I2C/I2C.h"
//#include "PESA/HX711.h"
void setup(void);
/****************************************/
// Globals
   // valor que el master leer?
volatile uint8_t buffer   = 0;   // ?ltimo byte recibido del master (por si us?s comandos)
volatile uint8_t DATOS = 0;
uint8_t estado;
/****************************************/
// Main

int main(void)
{
	
    // LED debug en PB5 (Arduino Nano: D13)
    DDRB  |= (1<<DDB5);
	
	DDRD |= (1<<DDD0)|(1<<DDD2)|(1<<DDD3)|(1<<DDD4);
	PORTD &= ~((1<<PORTD0)|(1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4));
    // I2C Slave init
	setup();
    I2C_Slave_Init(Slaveadress);	
    while (1)
    {
		if(buffer == 'M'){
			PORTD |= (1<<PORTD0);
			PORTD &= ~(1<<PORTD2);
		}
		else if(buffer == 'L'){
			PORTD &= ~(1<<PORTD0);
			PORTD &= ~(1<<PORTD2);
		}	
    }
}
//NON INTERRUPTION SUBRUTINE/
void setup(void){
	cli();
	//PONEMOS PD2 COMO ENTRADA
	DDRD &= ~(1<<DDD1);
	PORTD |= (1<<PORTD1);
	//Configuramos la interrupcion de pinchage para portD
	PCICR |= (1<<PCIE2);
	//CONFIGURAMOS PARA PD1
	PCMSK2 |= (1<<PCINT17);
	sei();
}

/****************************************/
// ISR: TWI (I2C) SLAVE
ISR(TWI_vect)
{
    estado = (TWSR & 0xF8);   // ? m?scara correcta: status en bits 7..3
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
            buffer = TWDR;  // guardo lo que envi? el master
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;
        // --- MASTER <- SLAVE (SLA+R) ---
        case 0xA8: // SLA+R recibido, ACK devuelto
        case 0xB8: // dato transmitido, ACK recibido (piden otro byte)
				TWDR = DATOS; //  (1 byte)
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
			
            break;

        // --- Fin de lectura (NACK) / fin de transmisi?n ---
        case 0xC0: // dato transmitido, NACK recibido (ya no quieren m?s)
        case 0xC8: // ?ltimo dato transmitido, ACK recibido
            // volver a modo ?listo/escucha?
			TWCR = 0;
			DATOS = 0;
            TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        // --- STOP o Repeated START detectado ---
        case 0xA0:
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        default:
            // recuperaci?n segura
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;
    }
}

/****************************************/
//FUNCIONES DE INTERRUPCION
ISR(PCINT2_vect){
	if (!(PIND & (1<<PIND1))){
			DATOS = 'D';
	}
	else DATOS = 0X00;
}

