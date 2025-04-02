/*
 * Prelab4.c
 *
 * Created: 1/04/2025
 * Author: Rodrigo Estrada
 * Description: prelab 4
 */
//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t multiplaxado; 
volatile uint8_t incremento;
//
// Function prototypes
void setup();
//
// Main Function
int main(void)
{
	
	//----Configuracion de Micro----//
	setup();
	while (1)
	{
		if(multiplaxado == 0){
		PORTB |= (1 << PORTB4);
		PORTD = incremento;
		PORTB &= ~(1 << PORTB4);
		}
		else if(multiplaxado == 1){
			//PORTB |= (1 << PORTB3);
			//PORTD = 0x05;
			//PORTB &= ~(1 << PORTB3);
		}
		else{
			//PORTB |= (1 << PORTB2);
			//PORTD = 0x03;
			//PORTB &= ~(1 << PORTB2);
		}
	}
}
//
// NON-Interrupt subroutines
void setup(){
	//Desactivamos las interrupcion globales
	cli(); 
	
	//Configuramos el puerto D como salida
	DDRD = 0XFF;
	//Inicialmente todos apagados 
	PORTD = 0x00;
	
	//Configuramos el puerto B los primeros 2 bits como entrada y el resto como salida 
	DDRB = 0b11111100;
	//Apagamos leds en en los bits del 2 al 8 y activamos pull ups de los bits del 0 al 1 
	PORTB = 0B00000011;
	
	//Configuramos Prescaler de frecuencia del MCU al 1Mhz
	CLKPR = (1 << CLKPCE); //habilitamos los cambios 
	CLKPR = (1 << CLKPS2); //seleccionamos prescaler de 16
	
	//Configuramos las interrupciones de pinchange 
	TCCR0B = (1 << CS01) | (1 << CS00); //colocamos un prescaler de 64 para el timer 0
	TCNT0 = 178; //Le cargamos el valor 178 para que cuente 5ms
	//Configuramos la interrupcion del timer 0
	TIMSK0 = (1 << TOIE0); //Habilitamos las interrupciones del timer 0
	
	//Configuramos la interrupcion por pin change 
	PCMSK0 = (1 << PCINT0) | (1 << PCINT1); //Configuramos el pinchage para PB0 y PB! 
	PCICR = (1 << PCIE0); //Habilitamos las interrupciones por pinchange 
	
	sei();
	
}
//
// Interrupt routines
ISR(PCINT0_vect){
	if (~PINB & (1 << PB0)){
		incremento++;
	}
	else if(~PINB & (1 << PB1)){
		incremento--;
	}
	else{
		
	}
}

ISR(TIMER0_OVF_vect){
	TCNT0 = 178; 
	multiplaxado++; 
	if(multiplaxado == 4){
		multiplaxado = 0;
	}
	else{

	}
}




