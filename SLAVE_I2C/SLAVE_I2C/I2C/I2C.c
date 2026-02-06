/*
 * I2C.c
 *
 * Created: 5/02/2026 11:26:17
 *  Author: rodro
 */ 
#include "I2C.h"
void I2C_master_init(unsigned long SCL_clock, uint8_t prescaler){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	switch(prescaler){
		case 1:
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
		break;
		case 4:
			TWSR &= ~(1<<TWPS1);
			TWSR |= (1<<TWPS1);
		break;
		case 16:
			TWSR &= ~(1<<TWPS0);
			TWSR |= (1<<TWPS1);
		break;
		case 64:
			TWSR |= ((1<<TWPS1)|(1<<TWPS0));
		break;
		default:
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
			prescaler = 1;
		break;
		
	}
	TWBR = ((F_CPU/SCL_clock)/16)/(2*prescaler);//CALCULAR LA VELOCIDAD
	TWCR |= (1<<TWEN);//ACTIVAR INTERFAZ (TWI) 12C
}
//funcion de la comunicacion I2C
uint8_t I2C_master_start(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //MASTER, REINICIAR BANDERA INT, CONDICION DE START
	while(!(TWCR & (1<<TWINT))); //ESPERAMOS A QUE SE ENCIENDA LA BANDERA
	return ((TWSR & 0XF8) == 0X08); //NOS QUEDAMOS con los bits de estado TWI status 
}

uint8_t I2C_master_Reapetedstart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //MASTER, REINICIAR BANDERA INT, CONDICION DE START
	while(!(TWCR & (1<<TWINT))); //ESPERAMOS A QUE SE ENCIENDA LA BANDERA
	return ((TWSR & 0XF8) == 0X10); //NOS QUEDAMOS con los bits de estado TWI status
}

//funcion de parada a la comunicacion I2C
void I2C_master_stop(void){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
	while(TWCR & (1<< TWSTO));
	 
}
//FUNCION PARA TRANSMITIR DATOS DE MAESTRO A ESCLAVO
//ESTA FUNCION DEVOLVERA UN 0 SI ESCLAVO A RECIBIDO EL DATO
uint8_t master_write(uint8_t dato){
	uint8_t estado;
	 TWDR = dato; //CARGAR EL DATO
	 TWCR = (1<<TWEN)|(1<<TWINT); //INICIA LA SENCUENCIA DE ENVIO
	 
	 while(!(TWCR & (1<<TWINT))); //ESPERA LA FLAG DE  TWINT
	 estado = TWSR & 0XF8; //NOS QUEDAMOS CON LOS BITS DE TWIN STATUS
	 //VERIFICAR SI SE TRANSMITIO UNA SLA + W CON ACK, O UN DATO CON ACK
	 if(estado == 0x18 || estado == 0x28){
		 return 1;
	 }
	 else{
		 return estado;
	 }
	 
	  
}
uint8_t I2C_master_read(uint8_t *buffer, uint8_t ACK){
	uint8_t estado;
	
	if(ACK){
		//ack: quiero mas datos
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);//ACTIVAMOS I2C CON ENVIO DE ACK		
	}
	else{
		TWCR = (1<<TWINT)|(1<<TWEN); //ACTIVAMOS INTERFAZ I2C CON SIN ENVIO DE ACK
	}
	while (!(TWCR & (1<<TWINT)));
	
	estado = TWSR & 0XF8;
	
	if(ACK && estado != 0X50) return 0;
	if(!ACK && estado != 0X58) return 0;
	
	*buffer = TWDR;
	return 1;
}
void I2C_slave_init(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	
	TWAR = address << 1;
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
	
}
