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
#define Slaveadress 0x20
#include "I2C/I2C.h"
#include "TIMER1/TIMER1.h"
void setup(void);
/****************************************/
// Globals
   // valor que el master leer?
volatile uint8_t buffer   = 0;   // ?ltimo byte recibido del master (por si us?s comandos)
volatile uint8_t DATOS = 0;
uint8_t estado;
volatile uint8_t step;
volatile uint8_t flagstep;
/****************************************/
// Main

int main(void)
{
    // LED debug en PB5 (Arduino Nano: D13)
    DDRB  |= (1<<DDB5);
    PORTB &= ~(1<<PORTB5);
	
	DDRD |= (1<<DDD5)|(1<<DDD6)|(1<<DDD7)|(1<<DDD4);
	PORTD &= ~((1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7)|(1<<PORTD4));
    // I2C Slave init
	timer1_init(1024,49911);
	setup();
    I2C_Slave_Init(Slaveadress);	
    while (1)
    {
		if(buffer == 'O'){
			flagstep = 1;
		}
		else if(buffer == 'K'){
			flagstep = 0;
		}
		if(flagstep == 1){	
			if(step == 0){
			PORTD &= ~((1<<PORTD5)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7));
			PORTD |= (1<<PORTD4);
			}
			else if(step == 1){
				PORTD &= ~((1<<PORTD5)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7));
				PORTD |= (1<<PORTD5);
			}
			else if(step == 2){
				PORTD &= ~((1<<PORTD5)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7));
				PORTD |= (1<<PORTD6);
			}
			else if(step == 3){
				PORTD &= ~((1<<PORTD5)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7));
				PORTD |= (1<<PORTD7);
			}
		}
		else {
			PORTD &= ~((1<<PORTD5)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7));
		}
		
    }
}
//NON INTERRUPTION SUBRUTINE/
void setup(void){
	cli();
	//PONEMOS PD2 COMO ENTRADA
	DDRD &= ~(1<<DDD1);
	//PORTD |= (1<<PORTD1);
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
	PORTB |= (1<<PORTB5);
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
	//DATOS = 0;
	if (buffer == 'S'){
		if ((PIND & (1<<PIND1))){
			DATOS = 'A';
		}
	}
}
ISR(TIMER1_OVF_vect){
	TCNT1 = 49911;
	step++;
	if(step>3) step = 0;
}

