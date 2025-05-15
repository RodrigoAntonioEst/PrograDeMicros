/*
 * IncFile1.h
 *
 * Created: 6/05/2025 12:21:58
 *  Author: rodro
 */ 


#ifndef PWM_H_
#define PWM_H_
//establecemos los prototipos de las funciones
//para timer0
void PWM0CONFIG(uint8_t INVERT0, uint8_t MODO0,uint16_t Prescaler0);
//Duty cycle para OCR0A
void duty0A(uint8_t Val0A_inf, uint8_t Val0A_sup,uint8_t POT0A);
//Duty cycle para OCR0B
void duty0B(uint8_t Val0B_inf, uint8_t Val0B_sup, uint8_t POT0B);
//para timer1 
void PWM1CONGIF(uint8_t INVERT1, uint8_t MODO1, uint16_t prescaler1 ,uint16_t top1);
//Duty cycle OCR1A
void duty1A(uint16_t Val1A_inf, uint16_t val1A_sup, uint8_t POT1A);
//Duty cycle OCR1B
void duty1B(uint16_t Val1B_inf, uint16_t Val1B_sup, uint8_t POT1B);
//para timer2 
void PWM2CONFIG(uint8_t INVERT2,uint8_t MODO2,uint16_t prescaler2);
//Duty cycle OCR2A
void duty2A(uint8_t val2A_inf, uint8_t val2A_sup, uint8_t POT2A);
//Funcion de escritura en eeprom
void write(uint16_t dirrecion, uint8_t dato);
//funcion de lectura eeprom 
uint8_t read(uint16_t dirrecion);


#endif /* PWM_H_ */