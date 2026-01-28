/*
 * ADC.h
 *
 * Created: 24/01/2026 23:34:33
 *  Author: rodro
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
//Prototipo de configuracion del ADC
void init_ADC(uint8_t justificacion, uint8_t prescaler, uint8_t pin_adc);
//funcion para seleccionar el pin del ADC
void pinADC(uint8_t pin_adc, uint8_t just);



#endif /* ADC_H_ */