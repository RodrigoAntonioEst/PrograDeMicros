/*
 * ProyectoGarra_micros.c
 *
 * Created: 5/05/2025 18:45:24
 * Author : Rodrigo Estrada 
 */ 

//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM/PWM.h"
volatile uint8_t MULTIPLEXACION;
volatile uint8_t POT0;
volatile uint8_t POT1;
volatile uint8_t POT2;
volatile uint8_t POT3;	
volatile uint8_t POT5;	
volatile uint8_t POT4;
volatile uint8_t MODOS;
volatile uint8_t POSICION1;
volatile uint8_t POSICION2;
volatile uint8_t POSICION3;
volatile uint8_t POSICION4;
volatile uint8_t POSICION5;
volatile uint8_t POSICION1_LEIDA;
volatile uint8_t POSICION2_LEIDA;
volatile uint8_t POSICION3_LEIDA;
volatile uint8_t POSICION4_LEIDA;
volatile uint8_t POSICION5_LEIDA;

//
// Function prototypes
void setup();
//configuracion de PWM de TIMER0
void PWM0CONFIG(uint8_t INVERT0, uint8_t MODO0 ,uint16_t Prescaler0);
//duty cycle para OCR0A
void duty0A(uint8_t Val0A_inf, uint8_t Val0A_sup,uint8_t POT0A);
//duty cycle para OCR0B
void duty0B(uint8_t Val0B_inf, uint8_t Val0B_sup, uint8_t POT0B);
//Configuracion PWM de TIMER1
void PWM1CONGIF(uint8_t INVERT1, uint8_t MODO1, uint16_t prescaler1 ,uint16_t top1);
//Duty cycle para OCR1A
void duty1A(uint16_t Val1A_inf, uint16_t val1A_sup,uint8_t POT1A);
//Duty cycle para OCR1B
void duty1B(uint16_t Val1B_inf, uint16_t Val1B_sup,uint8_t POT1B);
//Configuracion de PWM de TIMER2
void PWM2CONFIG(uint8_t INVERT2,uint8_t MODO2,uint16_t prescaler2);
//Duty cycle para OCR2A
void duty2A(uint8_t val2A_inf, uint8_t val2A_sup, uint8_t POT2A);
//Funcion de escritura en eeprom 
void write(uint16_t dirrecion, uint8_t dato);
//Funcion de lectura eeprom 
uint8_t read(uint16_t dirrecion);
//
// Main Function
int main(void)
{
	setup();
	PWM0CONFIG(1,1,64);
	PWM1CONGIF(1,1,64,312);
	PWM2CONFIG(1,1,64);
	while (1)
	{
		
		switch(MODOS){
			case 0:
				//apagamos indicador de modos 
				PORTB &= ~(1<<PORTB5);
				PORTB &= ~(1<<PORTB4);
			break;
			case 1:
				PORTB &= ~(1<<PORTB4);
				//indicador de modo guardado 
				PORTB |= (1<<PORTB5);
				//modo de guardado de poscision. 
				write(0X01,POSICION1);
				write(0X02,POSICION2);
				write(0X03,POSICION3);
				write(0X04,POSICION4);
				write(0X05,POSICION5);
			break;
			case 2:
				//apagamos indicador de guardado 
				PORTB &= ~(1<<PORTB5);
				//indicador de modo reproduccion 
				PORTB |= (1<<PORTB4);
				//modo de lectura de posicion guardada 
				POSICION1_LEIDA = read(1);
				duty2A(7,37,POSICION1_LEIDA);
				POSICION2_LEIDA = read(2);
				duty1A(7,37,POSICION2_LEIDA);
				POSICION3_LEIDA = read(3);
				duty1B(7,37, POSICION3_LEIDA);
				POSICION4_LEIDA = read(4);
				duty0A(7,37,POSICION4_LEIDA);
				POSICION5_LEIDA = read(5);
				duty0B(7,37, POSICION5_LEIDA);
			break;
			case 3:
				//indicador modo adafruit
				PORTB |= (1<<PORTB4);
				PORTB |= (1<<PORTB5);
				//aqui se pondra el modo adafruit
			break;
			default:
				MODOS = 0;
			break;
		}
	}
}
//
// NON-Interrupt subroutines
void setup(){
	cli();
	
	//Configuramos la frecuencia de micro a 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//Configuramos pines de entrada  
	DDRC &= ~(1<<DDC5);
	DDRD &= ~((1<<DDD2)|(1<<DDD4)|(1<<DDD7)); 
	
	//Activamos los pullups
	PORTC |= (1<<PORTC5);
	PORTD |= (1<<PORTD2) | (1<<PORTD4) | (1<<PORTD7);
	
	//establecemos pines como salida
	DDRB |= (1<<DDB5);  
	DDRB |= (1<<DDB4);
	
	//Apagamos esos pines 
	PORTB &= ~(1<<PORTB5);
	PORTB &= ~(1<<PORTB4);
	
	//configuramos el pinchange
	//Para PORTB y PORTD
	PCICR |=  (1<<PCIE1)|(1<<PCIE2);
	PCMSK1 |= (1<<PCINT13);
	PCMSK2 |= (1<<PCINT23)|(1<<PCINT20)|(1<<PCINT18);
	
	//configuramos la lectura para el ADC0
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	
	//CONFIGURACION ADC
	ADCSRA = 0; //eliminamos las configuraciones previos del ADC
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC); //activamos prescaler, interrupcion e iniciamos la conversion
	
	
	sei();
}
//
// Interrupt routines
ISR(ADC_vect){
	if(MODOS==0){
	switch(MULTIPLEXACION){
		case 1:
			POT1 = ADCH;
			POSICION1 = ADCH;
			duty2A(7,37,POT1);
			//configuramos la lectura para ADC1
			ADMUX = 0; //Asegurammos que se borre cualquier configuracion previa
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1<<MUX0);
		break;
		case 2:
			POT2 = ADCH;
			POSICION2 = ADCH;
			duty1A(7,37,POT2);
			//configuramos la lectura para el ADC2
			ADMUX = 0;
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1);
		break;
		case 3:
			POT3 = ADCH;
			POSICION3 = ADCH;
			duty1B(7,37,POT3);
			//configuramos la lectura del ADC3 
			ADMUX=0;
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0) | (1 << MUX1);
		break;
		case 4:
			POT4 = ADCH;
			POSICION4 = ADCH;
			duty0A(7,37,POT4);
			//configuramos la lectura del ADC4
			ADMUX=0;
			ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX2);
			break;
		case 5:
			POT5 = ADCH;
			POSICION5 = ADCH;
			duty0B(7,37, POT5);
			break;
		default:
			ADMUX = 0;
			//configuramos la lectura para el ADC0
			ADMUX |= (1 << REFS0) | (1 << ADLAR);
			MULTIPLEXACION = 0;
		break;
	}
	MULTIPLEXACION++; //incrementamos el contador cada vez que se termina de hacer la conversion del ADC
}
	ADCSRA |= (1 << ADSC);
}
ISR(PCINT1_vect){
	if (~PINC & (1<<PORTC5)){
		MODOS=0;
	}
}
ISR(PCINT2_vect){
	if(~PIND & (1<<PORTD2)){
		MODOS=2;
	}
	else if(~PIND & (1<<PORTD4)){
		MODOS=1;
	}
	else if(~PIND & (1<<PORTD7)){
		MODOS=3;
	}
}



