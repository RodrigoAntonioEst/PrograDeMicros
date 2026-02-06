/*
 * SPI.c
 *
 * Created: 28/01/2026 22:25:47
 *  Author: rodro
 */ 
#include "SPI.h"

void SPIinit(SPI_TYPE stype, SPI_DATA_ORDER	sDataOrder, SPI_CLOCK_POLARITY sClockPolarity, SPI_CLOCK_PHASE sClockPhase){
	 //PB2 ---> SS
	 //PB3 ---> MOSI
	 //PB4 ---> MISO
	 //PB5 ---> SCK
	 
	 if(stype & (1<<MSTR)){
		 DDRB |= (1<<DDB3)|(1<<DDB5)|(1<<DDB2); //MOSI. SCK, NEGADO_SS
		 DDRB &= ~(1<<DDB4);
		 SPCR |= (1<<MSTR);
		 
		  uint8_t temp = stype & 0b00000111;
		  switch(temp){
			  case 0: //DIV2
			  SPCR &= ~((1<<SPR1)|(1<<SPR0));
			  SPSR |= (1<<SPI2X);
			  break;
			  case 1: //DIV 4
			  SPCR &= ~((1<<SPR1)|(1<<SPR0));
			  SPSR &= ~(1<<SPI2X);
			  break;
			  case 2: //DIV8
			  SPCR |= (1<<SPR0);
			  SPCR &= ~(1<<SPR1);
			  SPSR |= (1<<SPI2X);
			  break;
			  case 3: //DIV16
			  SPCR |= (1<<SPR0);
			  SPCR &= ~(1<<SPR1);
			  SPSR &= ~(1<<SPI2X);
			  break;
			  case 4: //DIV32
			  SPCR &= ~(1<<SPR0);
			  SPCR |= (1<<SPR1);
			  SPSR |= (1<<SPI2X);
			  break;
			  case 5: //DIV64
			  SPCR &= ~(1<<SPR0);
			  SPCR |= (1<<SPR1);
			  SPSR &= ~(1<<SPI2X);
			  break;
			  case 6: //DIV128
			  SPCR |= (1<<SPR0)|(1<<SPR1);
			  SPSR &= ~(1<<SPI2X);
			  break;
		  }
	 }
	 else{ //IF slave mode
		  
		  DDRB |= (1<<DDB4); //MISO
		  DDRB &= ~((1<<DDB3)|(1<<DDB5)|(1<<DDB2)); //MOSI, SCK, SS
		  SPCR &= ~(1<<MSTR); //SLAVE
	 }
	 //ENABLE SPI, DATA ORDER, CLOCK POLARITY, CLOCK PHASE 
	 SPCR |= (1<<SPE)|sDataOrder|sClockPolarity|sClockPhase;
}
static void spiReceivewait(){ // Espera a que la informacion sea recibida por completo
	while (!(SPSR & (1<<SPIF)));
}
void SPIwrite(uint8_t dat){ //escribir datos al bus de SPI
	SPDR = dat;
}
unsigned SPIdataready(){ //ver si la info esta lista para ser leida
	if(SPSR & (1<<SPIF))
	return 1;
	else
	return 0;
}
uint8_t spiRead(void){ //lee la informacion recibida
	while(!(SPSR & (1<<SPIF))); //esperar a la informacion este completa
	return(SPDR); //leer la informacion recibida desde el buffer
}

uint8_t SPItransfer(uint8_t dat){
    SPDR = dat;                         // carga byte a enviar
    while(!(SPSR & (1<<SPIF)));         // espera fin de transferencia
    return SPDR;                        // devuelve lo recibido
}
