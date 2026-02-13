#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"

#define SLAVE2_ADDR 0x20

uint8_t buffer;

int main(void)
{
	

	I2C_Slave_Init(SLAVE2_ADDR);

	sei();

	while (1)
	{
	}
}



ISR(TWI_vect)
{
	uint8_t estado = TWSR & 0xF8;

	switch(estado)
	{
		case 0x60:  // SLA+W se recibió y ACK se envió
		case 0x70:  // General Call se recibió ACK se envió
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;

		case 0x80:  // Se recibió dato con previa dirección y se envió ACK
		case 0x90:  // Se recibió un generall call con previa dirección y se envió ACK
		buffer = TWDR;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;

		case 0xA8:  // SLA+R recibido, se envía ACK
		case 0xB8:  // esclavo envía dato, se recibió ACK
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;

		case 0xC0:
		case 0xC8:
		case 0xA0:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;

		default:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		break;
	}
}