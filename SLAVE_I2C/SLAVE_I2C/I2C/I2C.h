/*
 * I2C.h
 *
 * Created: 5/02/2026 11:26:03
 *  Author: rodro
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

//funcion para inicializar el maestro
void I2C_master_init(unsigned long SCL_clock, uint8_t prescaler);
//funcion para iniciar comunicacion I2C
uint8_t I2C_master_start(void);
uint8_t I2C_master_Reapetedstart(void);
//Funcion de parada de I2C
void I2C_master_stop(void);
//funcion para escribir de maestro a esclavo, esta funcion va a devolver un 0 si el esclavo recibio el dato.
uint8_t master_write(uint8_t dato);
//funcion de recepcion de datos enviados por el esclavo al maestro esta funcion es para leer los datos que estan en el esclavo
uint8_t I2C_master_read(uint8_t *buffer, uint8_t ACK);
//funcion para inicializar I2C esclavo
void I2C_slave_init(uint8_t address);




#endif /* I2C_H_ */