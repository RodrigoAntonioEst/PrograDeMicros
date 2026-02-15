#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "I2C/I2C.h"
#include "TIMER1/TIMER1.h"
#include "UART/UART_RECEIVER.h"
#include "HX711/HX711.h"
#include <stdlib.h>

#define SLAVE1		0x10
#define SLAVE2		0x20

#define SLAVE2_R   (0x20 << 1) | 0x01
#define SLAVE2_W   (0x20 << 1) & 0b11111110

#define SLAVE1_R   (0x10 << 1) | 0x01
#define SLAVE1_W   (0x10 << 1) & 0b11111110

uint8_t rxdato1;
volatile uint8_t step = 0;
volatile uint8_t flagstep = 0;
volatile uint8_t motorflag = 0;
uint8_t rawdata1 = 0;
uint8_t rawdata2 = 0;
uint8_t rawdata3 = 0;
int32_t datosprocesados = 0;
HX711_Cal cal = {.offset = 0, .scale = 420.0f};
float masa = 0;
uint8_t tare_flag = 0;
char buffer[20];
char out[40];
char masa_str[16];

int main(void)
{
	UART_RECEIVER(103);
	I2C_Master_Init(100000,1);
	DDRB |= (1<<DDB5);
	DDRC |= (1<<DDC1);
	PORTC &= ~(1<<PORTC1);
	sei();
	HX711_tare_fromRaw(&cal, datosprocesados);
	while (1)
	{
		//funcion para pedir peso
		if(!I2C_Master_Start()) return;

		if(!I2C_Master_Write(SLAVE2_W))
		{
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Write('W');

		if(!I2C_Master_RepeatedStart())
		{
			I2C_Master_Stop();
			return;
		}

		if(!I2C_Master_Write(SLAVE2_R)){
			I2C_Master_Stop();
			return;
		}

		I2C_Master_Read(&rawdata1,1);
		I2C_Master_Read(&rawdata2,1);
		I2C_Master_Read(&rawdata3,0);
		I2C_Master_Stop();
		datosprocesados = HX711_decode24(rawdata1,rawdata2,rawdata3);
		if(tare_flag == 0){
			HX711_tare_fromRaw(&cal, datosprocesados);
			tare_flag = 1;
		}
		masa = HX711_toUnits(&cal,datosprocesados);
		dtostrf(masa, 0, 2, masa_str);
		snprintf(out, sizeof(out), "Masa: %s g\r\n", masa_str);
		cadena(out);
		//Rutia para encender el stepper -------------------------------------------------------------------
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