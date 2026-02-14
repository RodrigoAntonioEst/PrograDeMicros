#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "I2C/I2C.h"
#include "TIMER1/TIMER1.h"

#define SLAVE1		0x10
#define SLAVE2		0x20

#define SLAVE2_R   (0x20 << 1) | 0x01
#define SLAVE2_W   (0x20 << 1) & 0b11111110

#define SLAVE1_R   (0x10 << 1) | 0x01
#define SLAVE1_W   (0x10 << 1) & 0b11111110

uint8_t rxdato1;
uint8_t rxdato2;
volatile uint8_t step = 0;
volatile uint8_t flagstep = 0;
volatile uint8_t motorflag = 0;


int main(void)
{
	I2C_Master_Init(100000,1);
	DDRB |= (1<<DDB5);
	DDRC |= (1<<DDC1);
	PORTC &= ~(1<<PORTC1);
	sei();
	while (1)
	{
		
		if(!I2C_Master_Start()) return;

		if(!I2C_Master_Write(SLAVE1_W))
		{
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Write('D');

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

		if(rxdato1 == 'D'){ 
			flagstep = 'O';
			motorflag = 'L';
			
		}
		else PORTC &= ~(1<<PORTC1);
		//rutina para regresar el elevador
		if(!I2C_Master_Start()) return;
		
		if(!I2C_Master_Write(SLAVE1_W))
		{
			I2C_Master_Stop();
			return;
		}
		I2C_Master_Write(motorflag);
		I2C_Master_Stop();
		
		//Rutina para mandar el valor de step
		
		if(!I2C_Master_Start()) return;

		if(!I2C_Master_Write(SLAVE2_W))
		{
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Write('S');

		if(!I2C_Master_RepeatedStart())
		{
			I2C_Master_Stop();
			return;
		}

		if(!I2C_Master_Write(SLAVE2_R)){
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Read(&rxdato1,0);
		I2C_Master_Stop();

		if(rxdato1 == 'A'){
			flagstep = 'K';
			motorflag = 'M';
		}
		else PORTC &= ~(1<<PORTC1);
		
		PORTB  &= ~(1<<PORTB5);
		
		//RUTINA PARA APAGAR O ENCENDER EL STEPPER
		if(!I2C_Master_Start()) return;
		
		if(!I2C_Master_Write(SLAVE2_W))
		{
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Write(flagstep);
		I2C_Master_Stop();
		
		//RUTINA PARA ENCENDER EL ELEVADOR
		if(!I2C_Master_Start()) return;
		
		if(!I2C_Master_Write(SLAVE1_W))
		{
			I2C_Master_Stop();
			return;
		}
		I2C_Master_Write(motorflag);
		I2C_Master_Stop();
	}
	
}
