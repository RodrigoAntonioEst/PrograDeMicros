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
#include "TIMER0/TIMER0.h"
/****************************************/
// Function prototypes
void setup(void);

//Variables
volatile uint8_t POT_SELECT = 0;
volatile uint8_t POTENCIOMETRO1 = 0;
volatile uint8_t POTENCIOMETRO2 = 0;
char buffer[4];
volatile uint8_t ANTIREBOTE1 = 0;
volatile uint8_t ANTIREBOTE2 = 0;
volatile uint8_t ANTIREBOTE3 = 0;
volatile uint8_t ANTIREBOTE4 = 0;
volatile uint8_t ANTIREBOTE5 = 0;
volatile uint8_t ANTIREBOTE6 = 0;
volatile uint8_t estadoPB1 = 1;
volatile uint8_t estadoPB2 = 1;
volatile uint8_t estadoPB3 = 1;
volatile uint8_t estadoPB4 = 1;
volatile uint8_t estadoPB5 = 1;
volatile uint8_t estadoPB6 = 1;
/****************************************/
// Main Function
int main(void)
{
	setup();
	init_ADC(1,128,0);
	UART_INIT(103);
	TIMER0_INIT(0,64,6);
	/* Replace with your application code */
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	//Desactivamos interrupciones globales
	cli();
	//declaramos como entrada
	DDRD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7)); 
	//configuramos el pullup
	PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7);
	//------------- Configuracion de pinchage --------------//
	//activamos pines deseados
	PCMSK2 = (1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22)|(1<<PCINT23);
	//Activamos interrupcion
	PCICR = (1<<PCIE2);
	//activamos interrupciones globales
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
		if(POTENCIOMETRO1 < 100){
			writechar('1');
			cadena("U"); // abajo
			cadena("\n");
		}
		else if(POTENCIOMETRO1 > 160){
			writechar('1');
			cadena("D"); // arriba
			cadena("\n");
		}
		
		break;
		case 1:
		//Realizamos lectura del potenciometro 2
		pinADC(0,1); //se configura que canal del ADC queremos leer
		POTENCIOMETRO2 = ADCH; //Se carga el valor leido por el ADC a la variable.
		if(POTENCIOMETRO2 < 100){
			writechar('1');
			cadena("L"); // izquierda
			cadena("\n");
		}
		else if(POTENCIOMETRO2 > 160){
			writechar('1');
			cadena("R"); // derecha
			cadena("\n");
		}
		break;
		default:
		//NADA
		break;
	}
	//luego de cada vuelta se aumenta el contador para leer el pot indicado
	POT_SELECT++;
	if(POT_SELECT > 1) POT_SELECT = 0; //reiniciamos pot select
	//se inicia la conversion otra vez
	ADCSRA |= (1 << ADSC);
}
ISR(TIMER0_OVF_vect){
	//Se cuenta cada 1 milisegundo para poder hacer la secuencia de antirebote digital
	if(ANTIREBOTE1 > 0) ANTIREBOTE1--;
	if(ANTIREBOTE2 > 0) ANTIREBOTE2--;
	if(ANTIREBOTE3 > 0) ANTIREBOTE3--;
	if(ANTIREBOTE4 > 0) ANTIREBOTE4--;
	if(ANTIREBOTE5 > 0) ANTIREBOTE5--;
	if(ANTIREBOTE6 > 0) ANTIREBOTE6--;
	TCNT0 = 6;
}
ISR(PCINT2_vect){
	//Rutina para leer los botones desde D2 hasta D7
	if(!(PIND & (1 << PIND2)) && ANTIREBOTE6 == 0){
		ANTIREBOTE1 = 65;
		writechar('1');
		cadena("A");
		cadena("\n");
	}
	if(!(PIND & (1 << PIND3)) && ANTIREBOTE6 == 0){
		ANTIREBOTE2 = 50;
		writechar('1');
		cadena("B");
		cadena("\n");
	}
	if(!(PIND & (1 << PIND4)) && ANTIREBOTE6 == 0){
		ANTIREBOTE3 = 50;
		writechar('1');
		cadena("Y");
		cadena("\n");
	}
	if(!(PIND & (1 << PIND5)) && ANTIREBOTE6 == 0){
		ANTIREBOTE4 = 50;
		writechar('1');
		cadena("X");
		cadena("\n");
		
	}
	if(!(PIND & (1 << PIND6)) && ANTIREBOTE6 == 0){
		ANTIREBOTE5 = 50;
		writechar('1');
		cadena("W");
		cadena("\n");
		
	}
	if(!(PIND & (1 << PIND7)) && ANTIREBOTE6 == 0){
		ANTIREBOTE6 = 65;
		writechar('1');
		cadena("Z");
		cadena("\n");
	}
}