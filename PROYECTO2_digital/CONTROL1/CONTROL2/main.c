/*
 * CONTROL2.c
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
void enviarComando(char control, char comando);
void actualizarDireccion(void);

// Variables
volatile uint8_t POT_SELECT = 0;
volatile uint8_t POTENCIOMETRO1 = 0;
volatile uint8_t POTENCIOMETRO2 = 0;
char buffer[4];

volatile uint8_t ANTIREBOTE1 = 0;   // A
volatile uint8_t ANTIREBOTE2 = 0;   // B
volatile uint8_t ANTIREBOTE3 = 0;
volatile uint8_t ANTIREBOTE4 = 0;
volatile uint8_t ANTIREBOTE5 = 0;
volatile uint8_t ANTIREBOTE6 = 0;

// Estados anteriores para detectar flancos
volatile uint8_t prevDir = 'N';
volatile uint8_t prevB = 0;
volatile uint8_t prevA = 0;

/****************************************/
// Main Function
int main(void)
{
	setup();
	init_ADC(1,128,0);
	UART_INIT(103);          // 9600 baud
	TIMER0_INIT(0,64,6);

	while (1)
	{
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();

	// -------- PORTD: D2, D3, D4 y D5 como entradas --------
	DDRD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5));
	PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5);

	// -------- PORTC: PC3 y PC4 como entradas --------
	DDRC &= ~((1<<DDC3)|(1<<DDC4));
	PORTC |= (1<<PORTC3)|(1<<PORTC4);

	// PORTD -> D2, D3, D4, D5
	PCMSK2 = (1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21);

	// PORTC -> C3, C4
	PCMSK1 = (1<<PCINT11)|(1<<PCINT12);

	// Activamos interrupciones en PORTD y PORTC
	PCICR = (1<<PCIE2)|(1<<PCIE1);

	sei();
}

void enviarComando(char control, char comando){
	writechar(control);
	writechar(comando);
	cadena("\n");
}


void actualizarDireccion(void){
	uint8_t dirActual = 'N';

	if(!(PIND & (1 << PIND4))){
		dirActual = 'U';
	}
	else if(!(PIND & (1 << PIND5))){
		dirActual = 'R';
	}
	else if(!(PIND & (1 << PIND2))){
		dirActual = 'D';
	}
	else if(!(PIND & (1 << PIND3))){
		dirActual = 'L';
	}
	else{
		dirActual = 'N';
	}

	if(dirActual != prevDir){
		prevDir = dirActual;
		enviarComando('2', dirActual);
	}
}

/****************************************/
// Interrupt routines
ISR(ADC_vect){
	switch(POT_SELECT){
		case 0:
			pinADC(1,1);
			POTENCIOMETRO1 = ADCH;
			break;

		case 1:
			pinADC(0,1);
			POTENCIOMETRO2 = ADCH;
			break;

		default:
			break;
	}

	POT_SELECT++;
	if(POT_SELECT > 1) POT_SELECT = 0;

	ADCSRA |= (1 << ADSC);
}

ISR(TIMER0_OVF_vect){
	// antirebote
	if(ANTIREBOTE1 > 0) ANTIREBOTE1--;
	if(ANTIREBOTE2 > 0) ANTIREBOTE2--;
	if(ANTIREBOTE3 > 0) ANTIREBOTE3--;
	if(ANTIREBOTE4 > 0) ANTIREBOTE4--;
	if(ANTIREBOTE5 > 0) ANTIREBOTE5--;
	if(ANTIREBOTE6 > 0) ANTIREBOTE6--;

	// Revisa continuamente el estado de dirección
	actualizarDireccion();

	// -------- B está en PC3 --------
	uint8_t bActual = (!(PINC & (1 << PINC3))) ? 1 : 0;
	if(bActual != prevB){
		prevB = bActual;

		if(bActual){
			enviarComando('2', 'B');
		}else{
			enviarComando('2', 'b');
		}
	}

	// -------- A está en PC4 --------
	uint8_t aActual = (!(PINC & (1 << PINC4))) ? 1 : 0;
	if(aActual != prevA){
		prevA = aActual;

		if(aActual){
			enviarComando('2', 'A');
		}else{
			enviarComando('2', 'a');
		}
	}

	TCNT0 = 6;
}

// Interrupcion para PORTD
ISR(PCINT2_vect){
	// refresca rápido el estado de dirección
	actualizarDireccion();

	// si se soltó dirección, mandar solo una N
	if((PIND & (1 << PIND2)) &&
	   (PIND & (1 << PIND3)) &&
	   (PIND & (1 << PIND4)) &&
	   (PIND & (1 << PIND5))){
		if(prevDir != 'N'){
			prevDir = 'N';
			enviarComando('2', 'N');
		}
	}
}

// Interrupcion para PORTC
ISR(PCINT1_vect){
	// -------- B en PC3 --------
	uint8_t bActual = (!(PINC & (1 << PINC3))) ? 1 : 0;
	if(bActual != prevB && ANTIREBOTE2 == 0){
		ANTIREBOTE2 = 40;
		prevB = bActual;

		if(bActual){
			enviarComando('2', 'B');
		}else{
			enviarComando('2', 'b');
		}
	}

	// -------- A en PC4 --------
	uint8_t aActual = (!(PINC & (1 << PINC4))) ? 1 : 0;
	if(aActual != prevA && ANTIREBOTE1 == 0){
		ANTIREBOTE1 = 40;
		prevA = aActual;

		if(aActual){
			enviarComando('2', 'A');
		}else{
			enviarComando('2', 'a');
		}
	}
}