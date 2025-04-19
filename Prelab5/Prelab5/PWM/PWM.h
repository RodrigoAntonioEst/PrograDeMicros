/*
 * PWM.h
 *
 * Created: 8/04/2025 23:44:57
 *  Author: rodro
 */ 


#ifndef PWM_H_
#define PWM_H_

//prototipo para la configuracion de PWM1
void PMW1CONFIG(uint16_t top, uint16_t prescaler);
//Prototipo para cambiar el ciclo de trabajo
void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP);
void PWM2CONFIG(uint16_t prescaler2);
void CICLODETRABJO0(uint16_t VAL0, uint16_t LIMITE_INF0, uint16_t LIMITE_SUP0);

#endif /* PWM_H_ */