/*
 * prelab6.c
 *
 * Created: 22/04/2025 22:47:38
 * Author : rodrigo
 */ 
//
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

//
// Function prototypes
void setup(void);
void UART_init(void);
void writechar(char caracter);
void cadena(char* frase);

//
// Variables globales
volatile uint8_t modo = 0;         // 1 = leer ADC, 2 = mostrar en LEDs
volatile uint8_t esperar_char = 0; // 1 = esperando carácter para LEDs
volatile uint16_t valorADC = 0;

//
// Main Function
int main(void)
{
	setup();
	UART_init();
	cadena("//----MENU HIPERTERMINAL----//\n");
	cadena("Mande 1 para leer potenciometro\n");
	cadena("Mande 2 para mostrar leds\n");
	while (1)
	{
	}
}

//
// NON-Interrupt subroutines
void setup(void){
	cli();

	DDRB = 0xFF;
	PORTB = 0x00;

	DDRC = 0x0F;
	PORTC = 0x00;

	// ADC7: usar AVcc como referencia, resultado justificado a la izquierda
	ADMUX = 0;
	ADMUX |= (1 << REFS0); // referencia AVCC
	ADMUX |= (1 << ADLAR); // justificar a la izquierda
	ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // seleccionar ADC7

	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // prescaler 8
	ADCSRA |= (1 << ADEN);  // habilitar ADC
	ADCSRA |= (1 << ADIE);  // habilitar interrupción ADC

	UART_init();

	sei();
}

void UART_init(void)
{
	DDRD |= (1<<DDD1); // TX
	DDRD &= ~(1<<DDD0); // RX
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = 103; // baudrate 9600 a 16MHz
}

void writechar(char caracter)
{
	while((UCSR0A & (1 << UDRE0))== 0);
	UDR0 = caracter;
}

void cadena(char* frase){
	for(uint8_t i = 0; *(frase+i) != '\0'; i++){
		writechar(*(frase+i));
	}
}

//
// Interrupt routines 
ISR(ADC_vect){
	valorADC = ADCH; // solo usamos los 8 bits altos

	if (modo == 1) {
		char buffer[10];
		itoa(valorADC, buffer, 10);
		cadena("Valor ADC: ");
		cadena(buffer);
		cadena("\n");

		modo = 0; // volver a modo normal
	}

	// No vuelvas a iniciar la conversión automáticamente aquí
}


ISR(USART_RX_vect){
	char dato = UDR0;

	if (!esperar_char) {
		if (dato == '1') {
			modo = 1;
			cadena("Modo: Lectura de potenciometro (ADC7)\n");
			ADCSRA |= (1 << ADSC); // iniciar conversión
		}
		else if (dato == '2') {
			modo = 2;
			esperar_char = 1;
			cadena("Modo: Mostrar LEDs. Envie un caracter...\n");
		}
		else {
			cadena("Comando no valido\n");
		}
		} else {
		// Mostrar en LEDs
		PORTB = 0x0F & dato;
		PORTC = 0x0F & (dato >> 4);
		cadena("Caracter recibido en LEDs\n");

		esperar_char = 0;
		modo = 0;
	}
}

