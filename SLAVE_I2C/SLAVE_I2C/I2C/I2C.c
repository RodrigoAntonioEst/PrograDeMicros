/*
 * I2C.c
 *
 * Created: 2/8/2026 12:11:33 AM
 *  Author: jargu
 */ 

#include "I2C.h"

// Función para inicializar I2C maestro 
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler){
    DDRC &= ~((1<<DDC4)|(1<<DDC5));       // PC4, PC5 -> SDA, SCL (salidas) 
    
    // Valor de prescaler para registro TWSR
    switch(Prescaler){
        case 1: 
            TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
        break;
        case 4: 
            TWSR &= ~(1<<TWPS1); 
            TWSR |= (1<< TWPS0);
        break; 
        case 16: 
            TWSR &= ~(1<<TWPS0);
            TWSR |= (1<< TWPS1);
        break;
        case 64: 
            TWSR |= ((1<<TWPS1)|(1<<TWPS0));
        break; 
        default:
            TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
            Prescaler = 1; 
        break;       
    }
    TWBR = ((F_CPU/SCL_Clock)-16)/(2*Prescaler); // selecciona el factor de división para el scl_clock
    TWCR |= (1<<TWEN);  // Activar interfaz I2C (TWI)     
    
       
}

// ----------------------------------------------------------------------------------------

// Función de inicio comunicación I2C 

uint8_t I2C_Master_Start(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);   
    while (!(TWCR & (1<<TWINT)));       // Esperamos a que encienda la bandera 
    
    return ((TWSR & 0XF8) == 0X08);     // START condition transmited 
}

// ----------------------------------------------------------------------------------------

uint8_t I2C_Master_RepeatedStart(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
    while (!(TWCR & (1<<TWINT)));       // Esperamos a que encienda la bandera
    
    return ((TWSR & 0XF8) == 0X10);     // START condition transmited
}

// ----------------------------------------------------------------------------------------

void I2C_Master_Stop(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); 
    while (TWCR & (1<<TWSTO));
}

// ----------------------------------------------------------------------------------------
// Funcion de transmisión de datos maestro -> esclavo 
// Si el esclavo YA recibió el dato, la función da 0 

uint8_t I2C_Master_Write(uint8_t dato){  // dato es lo que yo voy a mandar 
    uint8_t estado; 
    
    TWDR = dato; // cargar el dato 
    TWCR = (1<<TWEN)|(1<<TWINT); 
    
    while (!(TWCR &(1<<TWINT))); // Esperamos a que TWNT = 1
    estado = TWSR & 0XF8;        // Nos quedamos con los bits de estado
    
    if (estado == 0x18 || estado == 0x28)   // si se recibió ack después de enviar dirección o un byte de dato...
    {
        return 1;
    }
    else
    {
        return estado; 
    }    
}

// ----------------------------------------------------------------------------------------

// Función de recepción de datos esclavo -> maestro 
// Función para leer datos que están en el esclavo 

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
    uint8_t estado; 
    
    if (ack)
    {
        // ACK, quiero mas datos 
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); // habilitamos interfase I2C con envio de acknowledge 
    }
    else
    {
        // NACK, ultimo byte
        TWCR = (1<<TWINT)|(1<<TWEN); // habilitamos interfase I2C sin envio de acknowledge 
    }
    
    while (!(TWCR & (1<<TWINT)));   // esperar a interrupción 
    

        estado = TWSR & 0xF8;  // nos quedamos con los bits de estado 
        
        // verificar si se recibió Dato con ACK o NACK
        if (ack && estado != 0x50) return 0; 
        if (!ack && estado != 0x58) return 0; 
        
        *buffer = TWDR; // Obtenemos el contenido enviado por el esclavo 
        return 1; 
}

// ----------------------------------------------------------------------------------------

// Función para inicializar I2C eslcavo 
void I2C_Slave_Init(uint8_t adress){
    DDRC &= ~((1<<DDC4)|(1<<DDC5));     // Pines I2C de entrada 
    
    TWAR = adress << 1; // se asigna la dirección que tendrá, se corre 1 bit porque es de 7 bits
    
    // Se habilita la interfaz, ACK automatic0, activamos la ISR
    TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);  
}