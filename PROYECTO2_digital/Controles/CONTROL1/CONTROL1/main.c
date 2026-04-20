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

// Contadores para mantener repeticion mientras el boton siga presionado
volatile uint8_t HOLD1 = 0;
volatile uint8_t HOLD2 = 0;
volatile uint8_t HOLD3 = 0;
volatile uint8_t HOLD4 = 0;
volatile uint8_t HOLD5 = 0;
volatile uint8_t HOLD6 = 0;

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

	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();

	// -------- PORTD: solo D2, D3, D4 y D5 como entradas --------
	DDRD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5));
	PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5);

	// -------- PORTC: PC4 y PC5 como entradas --------
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	PORTC |= (1<<PORTC4)|(1<<PORTC5);

	// PORTD -> D2, D3, D4, D5
	PCMSK2 = (1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21);

	// PORTC -> C4, C5
	PCMSK1 = (1<<PCINT12)|(1<<PCINT13);

	// Activamos interrupciones en PORTD y PORTC
	PCICR = (1<<PCIE2)|(1<<PCIE1);

	sei();
}
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	switch(POT_SELECT){
		case 0:
		pinADC(1,1);
		POTENCIOMETRO1 = ADCH;
		if(POTENCIOMETRO1 < 100){
			//writechar('1');
			//cadena("U");
			//cadena("\n");
		}
		else if(POTENCIOMETRO1 > 160){
			//writechar('1');
			//cadena("D");
			//cadena("\n");
		}
		break;

		case 1:
		pinADC(0,1);
		POTENCIOMETRO2 = ADCH;
		if(POTENCIOMETRO2 < 100){
			//writechar('1');
			//cadena("L");
			//cadena("\n");
		}
		else if(POTENCIOMETRO2 > 160){
			//writechar('1');
			//cadena("R");
			//cadena("\n");
		}
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

	// ---------------- REPETICION MIENTRAS SIGA PRESIONADO ----------------
	// A -> PC5
	/*if(!(PINC & (1 << PINC5))){
		if(HOLD1 < 255) HOLD1++;
		if(HOLD1 >= 30){   // espera inicial
			writechar('1');
			cadena("A");
			cadena("\n");
			HOLD1 = 25;    // repeticion continua
		}
	}else{
		HOLD1 = 0;
	}

	// B -> PC4
	if(!(PINC & (1 << PINC4))){
		if(HOLD2 < 255) HOLD2++;
		if(HOLD2 >= 30){
			writechar('1');
			cadena("B");
			cadena("\n");
			HOLD2 = 25;
		}
	}else{
		HOLD2 = 0;
	}*/

	// Y -> D4
	if(!(PIND & (1 << PIND4))){
		if(HOLD3 < 255) HOLD3++;
		if(HOLD3 >= 30){
			writechar('1');
			cadena("U");
			cadena("\n");
			HOLD3 = 25;
		}
	}else{
		HOLD3 = 0;
	}

	// X -> D5
	if(!(PIND & (1 << PIND5))){
		if(HOLD4 < 255) HOLD4++;
		if(HOLD4 >= 30){
			writechar('1');
			cadena("R");
			cadena("\n");
			HOLD4 = 25;
		}
	}else{
		HOLD4 = 0;
	}

	// W -> D2
	if(!(PIND & (1 << PIND2))){
		if(HOLD5 < 255) HOLD5++;
		if(HOLD5 >= 30){
			writechar('1');
			cadena("D");
			cadena("\n");
			HOLD5 = 25;
		}
	}else{
		HOLD5 = 0;
	}

	// Z -> D3
	if(!(PIND & (1 << PIND3))){
		if(HOLD6 < 255) HOLD6++;
		if(HOLD6 >= 30){
			writechar('1');
			cadena("L");
			cadena("\n");
			HOLD6 = 25;
		}
	}else{
		HOLD6 = 0;
	}

	TCNT0 = 6;
}

// Interrupcion para PORTD
ISR(PCINT2_vect){
	// W en D2
	if(!(PIND & (1 << PIND2)) && ANTIREBOTE5 == 0){
		ANTIREBOTE5 = 100;
		HOLD5 = 0;
		writechar('1');
		cadena("D");
		cadena("\n");
	}

	// Z en D3
	if(!(PIND & (1 << PIND3)) && ANTIREBOTE6 == 0){
		ANTIREBOTE6 = 100;
		HOLD6 = 0;
		writechar('1');
		cadena("L");
		cadena("\n");
	}

	// Y en D4
	if(!(PIND & (1 << PIND4)) && ANTIREBOTE3 == 0){
		ANTIREBOTE3 = 100;
		HOLD3 = 0;
		writechar('1');
		cadena("U");
		cadena("\n");
	}

	// X en D5
	if(!(PIND & (1 << PIND5)) && ANTIREBOTE4 == 0){
		ANTIREBOTE4 = 100;
		HOLD4 = 0;
		writechar('1');
		cadena("R");
		cadena("\n");
	}
}

// Interrupcion para PORTC
ISR(PCINT1_vect){
	// B en PC4
	if(!(PINC & (1 << PINC4)) && ANTIREBOTE2 == 0){
		ANTIREBOTE2 = 100;
		HOLD2 = 0;
		writechar('1');
		cadena("B");
		cadena("\n");
	}

	// A en PC5
	if(!(PINC & (1 << PINC5)) && ANTIREBOTE1 == 0){
		ANTIREBOTE1 = 100;
		HOLD1 = 0;
		writechar('1');
		cadena("A");
		cadena("\n");
	}
}