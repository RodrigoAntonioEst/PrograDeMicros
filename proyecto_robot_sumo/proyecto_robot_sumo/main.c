/*
 * proyecto_sumobot.c
 *
 * Created: 22/08/2025
 * Author: Rodrigo Estrada
 * Description: proyecto sumo bot
 */
//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//
// Variable globales
volatile uint8_t MULTIPLEXACION;
volatile uint8_t EJE_X;
volatile uint8_t EJE_Y;
volatile uint8_t esperar_char = 0;
char buffer[10];
char buffer2[10];
//
// Function prototypes
void setup();
void writechar(char caracter);
void cadena(char* frase);

//
// Main Function
int main(void)
{
	setup();
	/* Replace with your application code */
	while (1)
	{
	}
}
//
// NON-Interrupt subroutines
//Establecemos como salidas los pines PC0 y PC1.
void setup(){
	cli();
	
	CLKPR = (1<<CLKPCE);
	CLKPR = (1<<CLKPS2);
	
	//ESTABLECEMOS PINES DE ENTRADA
	DDRC &= ~((1<<DDC0) | (1<<DDC1));
	DDRD &= ~(1<<DDD0);
	
	//ACTIVAMOS PULL UPS
	PORTC |= (1<<DDC0) | (1<<DDC1);
	PORTD |= (1<<DDD0);
	
	//ESTABLECEMOS PINES DE SALIDA
	DDRD |= (1<<DDD1) | (1<<DDD2);
	
	//APAGAMOS PINES 
	PORTD &= ~(1<<DDD1);
	
	//ENCIENDO PINES
	PORTD |= (1<<DDD2);
	
	//CONFIGURAMOS PRESCALER Y JUSTIFICACION DEL ADC
	ADMUX = 0; //BORRAMOS CUALQUIER CONFIGURACION PREVIA
	ADMUX |= (1<<REFS0) | (1 << ADLAR);
	
	//CONFIGURAMOS EL ADC
	ADCSRA = 0; //SE BORRA CUALQUIER CONFIGURACION PREVIA
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0) | (1 << ADIE) | (1<< ADEN) | (1 << ADSC); //ESTABLECEMOS EL PREESCALER, ACTIVAMOS ADC, INTERRUPCION Y CONVERSION 
	
	//CONFIGURAMOS LA TRANSMICION POR MEDIO DE UART
	UCSR0A = (1 << U2X0); //CONFIGURAMOS DOUBLE SPEED
	UBRR0 = 12; //CONFIGURAMOS BAUDRATE APROX A 9600
	
	//HABILITAR EL PIN PARA MANDAR INFORMACION
	UCSR0B = (1<<TXEN0);
	
	//FORMATO 8 BITS, 1 STOP BIT, SIN PARIDAD
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	
	
	
	sei();
}
//Rutina para poder mandar un caracter
void writechar(char caracter){
	while((UCSR0A & (1<<UDRE0)) == 0);
	UDR0 = caracter;
}

//Rutina para poder mandar una cadena
void cadena(char* frase){
	for(uint8_t i = 0; *(frase+i) !='\0'; i++){
		writechar(*(frase+i));
	}
}
//
// Interrupt routines
ISR(ADC_vect){
	switch(MULTIPLEXACION){
		case 1:
			EJE_X = ADCH;
			itoa(EJE_X, buffer, 10);
			cadena("EJE_X: ");
			cadena(buffer);
			cadena("\n");
			
			//configuramos la lectura para ADC1
			ADMUX = 0; //BORRAMOS CUALQUIER CONFIGURACION
			ADMUX |= (1<<REFS0) | (1<<ADLAR) | (1<<MUX0);
		break;
		default:
			EJE_Y = ADCH;
			itoa(EJE_Y, buffer2, 10);
			cadena("EJE_Y: ");
			cadena(buffer2);
			cadena("\n");
			ADMUX = 0;
			//configuramos lectura para ADC0
			ADMUX |= (1<<REFS0) | (1<<ADLAR);
			MULTIPLEXACION = 0;
		break;
	}	
	MULTIPLEXACION++;
	ADCSRA |= (1<<ADSC);
}




