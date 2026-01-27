/*
 * LAB2_DISPLAYLCD_DIGITAL2.c
 *
 * 
 * Author : rodro
 */ 
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
//llamamos a la libreria para poder utilizar display 16x2
#include "DISPLAYLCD/DISPLAYLCD.h"
//llamamos a la libreria del ADC
#include "ADC/ADC.h"
//llamamos la funcion de usart
#include "UART_RECEIVER/UART_RECEIVER.h"
volatile uint8_t MULTIPLEXADO = 0;
volatile uint16_t POT1;
volatile uint16_t POT2;
char t[16];
float ADC1;
float ADC2;
volatile uint8_t esperar_char = 0;
char dato;
volatile uint8_t contador = 0;
char buf[10];
/****************************************/
// Function prototypes

/****************************************/
// Main Function
int main(void)
{
	init8bits();
	UART_RECEIVER(103);
	init_ADC(1, 128, 3);
	cadena("Contador. Envie un '+' o '-'...\n");
	while (1)
	{
		
	}
}
/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines
ISR(ADC_vect){
	switch(MULTIPLEXADO){
		case 1:
			POT1 = ADCH;
			ADC1 = (POT1 * 5.00)/255.0;
			dtostrf(ADC1, 4, 2, t);
			LCD_SET_CURSOR(2,1);
			LCD_WRITE_STRING("S1");
			LCD_SET_CURSOR(1,2);
			LCD_WRITE_STRING(t);
			LCD_WRITE_STRING("V");
			pinADC(4);
		break;
		case 2:
			POT2 = ADCH;
			ADC2 = (POT2*5.00)/255.0;
			dtostrf(ADC2, 4, 2, t);
			LCD_SET_CURSOR(8,1);
			LCD_WRITE_STRING("S2");
			LCD_SET_CURSOR(7,2);
			LCD_WRITE_STRING(t);
			LCD_WRITE_STRING("V");
		break;
		default:
			MULTIPLEXADO = 0;
			pinADC(3);
		break;
	}
	if(contador == 0){
	LCD_SET_CURSOR(14,1);
	LCD_WRITE_STRING("S3");
	LCD_SET_CURSOR(14,2);
	LCD_WRITE_STRING("0");
	}
	MULTIPLEXADO++;
	ADCSRA |= (1 << ADSC);
}
ISR(USART_RX_vect){
	dato = UDR0;
	
	if(dato == '+'){
		contador++;
	}
	else if(dato == '-'){
		contador--;
	}
	else{
		cadena("NO VALIDO\n");
	}
	
	itoa(contador,buf, 10);
	LCD_SET_CURSOR(14,1);
	LCD_WRITE_STRING("S3");
	LCD_SET_CURSOR(14,2);
	LCD_WRITE_STRING(buf);
	LCD_WRITE_STRING("  ");
	cadena("Contador S3, Envie un '+' o '-'...\n");
	
}