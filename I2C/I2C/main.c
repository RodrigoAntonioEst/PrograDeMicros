#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "I2C/I2C.h"

#define SLAVE1		0x10
#define SLAVE2		0x20

#define SLAVE2_R   (0x20 << 1) | 0x01
#define SLAVE2_W   (0x20 << 1) & 0b11111110

#define SLAVE1_R   (0x10 << 1) | 0x01
#define SLAVE1_W   (0x10 << 1) & 0b11111110

uint8_t rxdato1;
uint8_t rxdato2;

int main(void)
{
	I2C_Master_Init(100000,1);
	DDRB |= (1<<DDB5);
	DDRC |= (1<<DDC1);
	PORTC &= ~(1<<PORTC1);

	while (1)
	{	
		PORTB |= (1<<PORTB5);
		
		if(!I2C_Master_Start()) return;

		if(!I2C_Master_Write(SLAVE1_W))
		{
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Write('R');

		if(!I2C_Master_RepeatedStart())
		{
			I2C_Master_Stop();
			return;
		}

		if(!I2C_Master_Write(SLAVE1_R)){
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Read(&rxdato1,0);
		I2C_Master_Stop();
		
		PORTB  &= ~(1<<PORTB5);

		if(rxdato1 == 0x01) PORTC |= (1<<PORTC1);
		else PORTC &= ~(1<<PORTC1);

		_delay_ms(100);
	}
}