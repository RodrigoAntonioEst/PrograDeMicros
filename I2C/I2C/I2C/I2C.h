#pragma once
#include <stdint.h>

void I2C_master_init(unsigned long SCL_clock, uint8_t prescaler);
uint8_t I2C_master_start(void);
uint8_t I2C_master_Reapetedstart(void);
void I2C_master_stop(void);

uint8_t I2C_master_sendAddress(uint8_t address7, uint8_t rw);
uint8_t I2C_master_writeByte(uint8_t data);
uint8_t I2C_master_read(uint8_t *buffer, uint8_t ACK);

void I2C_slave_init(uint8_t address);
