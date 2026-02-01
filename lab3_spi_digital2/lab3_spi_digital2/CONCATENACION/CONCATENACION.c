/*
 * CONCATENACION.c
 *
 * Created: 31/01/2026 18:41:39
 *  Author: rodro
 */ 
#include "CONCATENACION.h"
void concatenacion(char caracter){
	uint8_t info = caracter;
	return(info);
}

void  caden(char* uni){
	for(uint8_t i = 0; *(uni+i) != '\0'; i++){
		writechar(*(uni+i));
	}
}