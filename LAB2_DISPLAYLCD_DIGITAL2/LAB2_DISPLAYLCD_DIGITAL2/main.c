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
//llamamos a la libreria para poder utilizar display 16x2
#include "DISPLAYLCD/DISPLAYLCD.h"
//llamamos a la libreria para poder hacer 
#include "LECTURA_ADC/LECTURA_ADC.h"
/****************************************/
// Function prototypes

/****************************************/
// Main Function
int main(void)
{
	init8bits();
	LCD_SET_CURSOR(1,1);
	LCD_WRITE_CHAR('H');
	LCD_SET_CURSOR(1,2);
	LCD_WRITE_STRING("Mundo");
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines
