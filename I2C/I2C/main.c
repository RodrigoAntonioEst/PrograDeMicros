/*
 * MASTER_I2C.c
 *
 * Created: 5/02/2026
 * Author : rodro
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "I2C/I2C.h"

// Dirección del slave (7-bit)
#define SLAVE1_ADDR 0x30

static void refreshPORT(uint8_t valor);
static void setup(void);

int main(void)
{
    setup();
    I2C_master_init(100000UL, 1); // 100 kHz, prescaler 1

    uint8_t adc = 0;

    while (1)
    {
        // 1) START
        if(!I2C_master_start()){
            I2C_master_stop();
            continue;  // no mates el programa
        }

        // 2) SLA+W
        if(!I2C_master_sendAddress(SLAVE1_ADDR, 0)){ // 0=write
            I2C_master_stop();
            continue;
        }

        // 3) (opcional) comando 'R' para tu slave
        if(!I2C_master_writeByte('R')){
            I2C_master_stop();
            continue;
        }

        // 4) Repeated START
        if(!I2C_master_Reapetedstart()){
            I2C_master_stop();
            continue;
        }

        // 5) SLA+R
        if(!I2C_master_sendAddress(SLAVE1_ADDR, 1)){ // 1=read
            I2C_master_stop();
            continue;
        }

        // 6) Leer 1 byte y terminar con NACK
        if(!I2C_master_read(&adc, 0)){
            I2C_master_stop();
            continue;
        }

        // 7) STOP
        I2C_master_stop();

        // 8) Mostrar en LEDs
        refreshPORT(adc);

        _delay_ms(50);
    }
}

// ====== Tus mismas funciones ======
static void refreshPORT(uint8_t valor){
    if(valor & 0b10000000) PORTB |= (1<<PORTB1); else PORTB &= ~(1<<PORTB1);
    if(valor & 0b01000000) PORTB |= (1<<PORTB0); else PORTB &= ~(1<<PORTB0);
    if(valor & 0b00100000) PORTD |= (1<<PORTD7); else PORTD &= ~(1<<PORTD7);
    if(valor & 0b00010000) PORTD |= (1<<PORTD6); else PORTD &= ~(1<<PORTD6);
    if(valor & 0b00001000) PORTD |= (1<<PORTD5); else PORTD &= ~(1<<PORTD5);
    if(valor & 0b00000100) PORTD |= (1<<PORTD4); else PORTD &= ~(1<<PORTD4);
    if(valor & 0b00000010) PORTD |= (1<<PORTD3); else PORTD &= ~(1<<PORTD3);
    if(valor & 0b00000001) PORTD |= (1<<PORTD2); else PORTD &= ~(1<<PORTD2);
}

static void setup(){
    DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
    DDRB |= (1<<DDB0)|(1<<DDB1);

    PORTB &= ~((1<<PORTB0)|(1<<PORTB1));
    PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7));
}
