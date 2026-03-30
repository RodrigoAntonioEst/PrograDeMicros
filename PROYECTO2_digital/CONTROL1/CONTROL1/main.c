/*
 * CONTROL1.c
 *
 * Created: 26/03/2026 19:25:03
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "ADC/ADC.h"
#include "UART/UART.h"
/****************************************/
// Function prototypes
void setup(void);

//Variables
volatile uint8_t POT_SELECT = 0;
volatile uint8_t POTENCIOMETRO1 = 0;
volatile uint8_t POTENCIOMETRO2 = 0;
char buffer[4];
/****************************************/
// Main Function
int main(void)
{
	setup();
	init_ADC(1,128,0);
	UART_INIT(103);
	/* Replace with your application code */
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();

	sei();
}
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	switch(POT_SELECT){
		case 0:
		//Realizamos lectura del potenciometro 1
			pinADC(1,1); //se configura que canal del ADC queremos leer
			POTENCIOMETRO1 = ADCH; //Se carga el valor leido por el ADC a la variable.
			itoa(POTENCIOMETRO1 /*variable*/, buffer /*arreglo*/, 10 /*base*/);
			cadena("eje Y: ");
			cadena(buffer);
			cadena("\n");
		break;
		case 1:
		//Realizamos lectura del potenciometro 2
			pinADC(0,1); //se configura que canal del ADC queremos leer
			POTENCIOMETRO2 = ADCH; //Se carga el valor leido por el ADC a la variable.
			itoa(POTENCIOMETRO2 /*variable*/, buffer /*arreglo*/, 10 /*base*/);
			cadena("eje X: ");
			cadena(buffer);
			cadena("\n");
		break;
		default:
			//NADA
		break;
	}
	POT_SELECT++;
	if(POT_SELECT > 1) POT_SELECT = 0; //
	ADCSRA |= (1 << ADSC);
}