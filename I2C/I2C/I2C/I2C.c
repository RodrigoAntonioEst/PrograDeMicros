/*
 * I2C.c
 *
 * Created: 5/02/2026
 * Author : rodro
 */

#include "I2C.h"
#include <avr/io.h>

void I2C_master_init(unsigned long SCL_clock, uint8_t prescaler){
    DDRC &= ~((1<<DDC4)|(1<<DDC5)); // SDA/SCL entradas (pull-ups externos)

    // Prescaler bits TWPS1:0
    switch(prescaler){
        case 1:  TWSR &= ~((1<<TWPS1)|(1<<TWPS0)); break;
        case 4:  TWSR = (TWSR & ~((1<<TWPS1)|(1<<TWPS0))) | (1<<TWPS0); break;
        case 16: TWSR = (TWSR & ~((1<<TWPS1)|(1<<TWPS0))) | (1<<TWPS1); break;
        case 64: TWSR |=  (1<<TWPS1)|(1<<TWPS0); break;
        default: TWSR &= ~((1<<TWPS1)|(1<<TWPS0)); prescaler = 1; break;
    }

    TWBR = (uint8_t)((16000000/SCL_clock - 16) / (2*prescaler));
    TWCR = (1<<TWEN);
}

uint8_t I2C_master_start(void){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    return ((TWSR & 0xF8) == 0x08);
}

uint8_t I2C_master_Reapetedstart(void){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    return ((TWSR & 0xF8) == 0x10);
}

void I2C_master_stop(void){
    TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
    while(TWCR & (1<<TWSTO));
}

// ? Enviar dirección + R/W (maneja 0x18 y 0x40)
uint8_t I2C_master_sendAddress(uint8_t address7, uint8_t rw){
    TWDR = (address7 << 1) | (rw & 0x01);
    TWCR = (1<<TWEN)|(1<<TWINT);

    while(!(TWCR & (1<<TWINT)));

    uint8_t st = TWSR & 0xF8;

    if(rw == 0){
        // SLA+W ACK = 0x18
        return (st == 0x18);
    }else{
        // SLA+R ACK = 0x40
        return (st == 0x40);
    }
}

// ? Enviar un byte de datos (DATA ACK = 0x28)
uint8_t I2C_master_writeByte(uint8_t data){
    TWDR = data;
    TWCR = (1<<TWEN)|(1<<TWINT);

    while(!(TWCR & (1<<TWINT)));

    uint8_t st = TWSR & 0xF8;
    return (st == 0x28);
}

uint8_t I2C_master_read(uint8_t *buffer, uint8_t ACK){
    uint8_t estado;

    if(ACK){
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    }else{
        TWCR = (1<<TWINT)|(1<<TWEN); // NACK al final
    }

    while (!(TWCR & (1<<TWINT)));

    estado = TWSR & 0xF8;

    if(ACK && estado != 0x50) return 0;
    if(!ACK && estado != 0x58) return 0;

    *buffer = TWDR;
    return 1;
}
