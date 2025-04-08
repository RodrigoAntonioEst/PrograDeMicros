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
const uint8_t display[16] = {
	0b11000000, // 0
	0b11111001, // 1
	0b10100100, // 2
	0b10110000, // 3
	0b10011001, // 4
	0b10010010, // 5
	0b10000010, // 6
	0b11111000, // 7
	0b10000000, // 8
	0b10010000, // 9
	0b10001000, // A
	0b10000011, // b
	0b11000110, // C
	0b10100001, // d
	0b10000110, // E
	0b10001110  // F
};

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
		uint8_t valor = ADCH; 
		//se analizan los 4 bits menos significativos para poder mostrar las unidades
		uint8_t unidades_dis = valor & 0x0F;
		//se analizan los 4 bits mas significativos para las decenas, devido a que cada que las unidads hacen overflow se incrementan las decenas. 
		uint8_t decenas_dis = (valor >> 4) & 0x0F;
		//Hacemos hacemos la logica para poder encender la alarma si el ADC es mayor al contador de 8 bits 
		if (ADCH <= incremento){
			PORTB &= ~(1 << PORTB5);
		}
		else{
			PORTB |= (1 << PORTB5);
		}
		//Aqui se eejecuta la rutina para poder mostrar el contador de 8bits 
		if(multiplaxado == 0){
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
		PORTD = incremento;
		}
		//Aqui se ejecuta la rutina de las unidades del ADC
		else if(multiplaxado == 1){
			PORTB &= ~(1 << PORTB4);
			PORTB &= ~(1 << PORTB2);
			PORTB |= (1 << PORTB3);
			PORTD = display[unidades_dis];

		}
		//aqui se ejecuta la rutina de las decena del ADC
		else{
			PORTB &= ~(1 << PORTB3);
			PORTB &= ~(1 << PORTB4);
			PORTB |= (1 << PORTB2);
			PORTD = display[decenas_dis];
			
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
	
	//Configuramos el ADC,
	ADMUX = 0; //Realizamos est configuracion para poder elimninar cualquier configuracion previa
	ADMUX |= (1<<REFS0); //Utilizamos la referencia de los 5v 
	ADMUX |= (1<< ADLAR);	//usamos los bits mas significativos 
	ADMUX |= (1 << MUX0) | (1 << MUX1) | (1 << MUX2);
	
	ADCSRA = 0;
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADSC);
	
	sei();
	
}
//
// Interrupt routines
ISR(PCINT0_vect){
	//cada vez que PB0 se preciona aumenta. 
	if (~PINB & (1 << PORTB0)){
		incremento++;
	}
	//cada vez que PB1 se preciona se decrementa.
	else if(~PINB & (1 << PORTB1)){
		incremento--;
	}
	else{
		
	}
}

ISR(TIMER0_OVF_vect){
	TCNT0 = 178; //se recarga el valor del TIMER0
	//se incrementa cada que pasa overflow 
	multiplaxado++; 
	//runtina para multiplexar cada 5ms 
	if(multiplaxado == 3){
		multiplaxado = 0;
	}
	else{

	}
}

ISR(ADC_vect){
	//se recarga el enable del ADC 
	ADCSRA |= (1 << ADSC);
}




