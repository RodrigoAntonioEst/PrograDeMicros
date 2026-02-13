/*
 * I2C.h
 *
 * Created: 2/8/2026 12:11:47 AM
 *  Author: jargu
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU 
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>

// Funcion para inicializar maestro 
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler); // velocidad del reloj, prescaler 

// Funcion de inicio de la comunicación I2C 
uint8_t I2C_Master_Start(void); 
uint8_t I2C_Master_RepeatedStart(void);

// Función de parada de la comunicación I2C 
void I2C_Master_Stop(void);

// Función de transmisión de datos maestro -> esclavo. 
    // Devuelve un 0 si el esclavo YA recibió el dato
uint8_t I2C_Master_Write(uint8_t dato);

// Función de recepción de datos eslavo -> maestro 
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);

// Función para inicializar I2C esclavo 
void I2C_Slave_Init(uint8_t adress); 

#endif /* I2C_H_ */