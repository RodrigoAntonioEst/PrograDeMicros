/*
 * Lab1.c
 *
 * Created: 17-01-2026
 * Author: Rodrigo Estrada
 * Description: Juego de la carrera
 */
/**************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "7SEGMENTOS_ANODO.h"
volatile bool inicio = false;
volatile uint8_t valorfuncion;
//inicializamos estas variables para que no tengan basura
volatile uint8_t conteo = 0; //contedar para llevar la cuenta del segundo
volatile uint8_t numero = 0; //señala la posicion del array que quiero mostrar
volatile uint8_t jg1 = 0;  //jugador 1
volatile uint8_t jg2 = 0; //jugador 2
volatile uint8_t multiplexacion = 0; //para multiplexar contadores
volatile uint8_t R0 = 0;   // antirebote para PC0
volatile uint8_t R1 = 0;   // antirebote para PC1
volatile uint8_t R2 = 0;   // antirebote para PB1


/**************/
// Function prototypes
//configuracion del nano
void setup();
//funcion para mostrar el display de siete segmentos
void display_anodo(uint8_t posicion);
/**************/
// Main Function
int main(void)
{
	//se manda a llamar la configuracion del Atmega
	setup();
	while (1)
	{	
	}
}

/**************/
// NON-Interrupt subroutines
void setup(){
	cli();//Activamos las interrupciones globales
	//Habilitamos cambios en la frencuancia
	CLKPR = (1<<CLKPCE);
	//Seleccionamos pre scaler para dejar a 1MHZ la frecuencia
	CLKPR = (1 << CLKPS2);
	//Se configura el puerto D como salida
	DDRD = 0xFF;
	//Apgamos todos el display de 7 segmentos
	PORTD = 0XFF;
	//Configuramos el puerto C bits 0, 1 y 2 como entrada
	DDRC &= ~((1<<DDC0) | (1<<DDC1) | (1<<DDC2));
	//conmfiguramos bits 3 y 4 como salida
	DDRC |= (1<<DDC5) | (1<<DDC4);
	//Apagamos esos pines
	PORTC &= ~((1<<PORTC5)|(1<<PORTC4)); 
	//Activamos los pu llups en bits 0, 1 y 2 del port C
	PORTC |= (1<<PORTC0) | (1<<PORTC1) | (1<<PORTC2);
	//configuramos el PB0 como salida
	DDRB |= (1<<DDB0);
	//Apamos PB0
	PORTB &= ~(1<<PORTB0);
	//configuramos PB1 como entrada
	DDRB &= ~(1<<DDB1);
	//CONFIGURAMOS PULL UP
	PORTB |= (1<<PORTB1);
	//Configuramos pinchange para portC
	PCICR |= (1<<PCIE1);
	//Activamos pinchage para PC0, PC1, PC2
	PCMSK1 |= (1<<PCINT8) |(1<<PCINT9) | (1<<PCINT10);
	//configuramos pinchage para el portB
	PCICR |= (1<<PCIE0);
	//Activamos para PB1
	PCMSK0 |= (1<<PCINT1);
	//Configuramos timer0 para overflow cada 5ms
	TCCR0B = (1 << CS01) | (1 << CS00); //Prescaler de 64
	//configuramos TCNT0 a 178
	TCNT0 = 178;
	//Habilitamos interrupcion por overflow de timer 0
	TIMSK0 = (1<<TOIE0);
	//sacamos por el portd el numero 0 del array en el display
	PORTD = 0b10010010;
	
	sei();//Volvemos a habilitar las interrupciones globales
}
/**************/
// Interrupt routines
//interupcion por pinchage para el PORTC
ISR(PCINT1_vect){
	//Aqui entra a funcionarel antirebote con la lectura, lee que tenga pinc y luego hace una mascara con el contenido de el bit 0 de portc, luego lo niega para poder obtener un 1 logico
	if(!(PINC & (1<<PORTC0)) && R0 == 0){ 
		R0 = 10; //hacemos que cuente 50ms con el timer0 para que pueda volver a leer la interrupcion
		inicio = true; //esta es una bandera para saber cuando se inicio la cuenta atras
		//se limpian las variables para cuando se reinicie el juego
		numero = 0; 
		conteo = 0;
		jg1 = 0;
		jg2 = 0;
	}
	//se ejecuta la logica para poder leer el PC1
	else if(!(PINC & (1<<PORTC1)) && R1 == 0){
		R1 = 10; // se pone el delay de 50ms para evitar rebotes
		//aqui se aplica la logica que dice, cuando no esta contando hacia atras y a su vez solo deja aumentar el contador media vez no se ha ganado el juego
		if((!inicio && ((jg1 < 0x0F) && (jg2 < 0x0F)))){ 
			if(jg1 == 0){
				jg1++; //si la variable esta en 0 esta se le suma 1
			}
			else if(jg1 == 0b00000100){ //Se compara para ver si no se el jugador 1 ha ganado el juego
				jg1 = 0x0F; //se llenan las leds del jg1
				numero = 4; //se muestra el numero de jugador
				jg2 = 0; //se apagan todas las leds del jg2
			}
			else{
				jg1 <<= 1; //si todavia esta el juego activo se corre el bit a la izquierda
			}
		}
	}
}
ISR(PCINT0_vect){
	//se repite la logica para el portC
	if(!(PINB & (1<<PORTB1)) && R2 == 0){
		R2 = 10; //delay de 50ms
		if(!inicio && ((jg1 < 0x0F) && (jg2 < 0x0F))){
			if(jg2 == 0){
				jg2++; //si el jugador jg2 esta en 0 se le suma
			}
			else if(jg2 == 0b00000100){ //compara si el jugador 2 gano
				jg2 = 0x0F; //si gano llena los leds
				numero = 3; //se muestra su numero de ganador
				jg1 = 0; //se vacian los leds del jugador 2
			}
			else{
				jg2 <<= 1; //de la contrario se mueven los bits
			}
		}
	}
}
ISR(TIMER0_OVF_vect){
	//;as variables del antirebote se empiezan a restar hasta que estas mayores que 0
	 if(R0 > 0) R0--;
	 if(R1 > 0) R1--;
	 if(R2 > 0) R2--;
	multiplexacion++; //se le aumenta a la multiplexacion
	if(multiplexacion > 3){ //si la multiplexacion ya se paso de 3 
		multiplexacion = 0; //si se paso de 3 se reinicia a 0
	}
	conteo++; //este lleva el conteo, cada vez que sume 200 es un segundo
	if((conteo >= 200) && inicio){ //si la bandera de inicio es true y ya pasaron mas de 200 entra
		conteo = 0; //se resetea el conteo
		numero++; //se aumenta el numero que se quiere mostrar en el display
		if(numero == 5){ //si ya se llego al ultimo numero que es 0 se pone la bandera en false para que ya se pueda avanzar en el juego
			inicio = false;
			numero = 5;
		}
	}
	//rutina de multiplexado
	PORTB &= ~(1<<PORTB0);
	PORTC &= ~(1<<PORTC4);
	PORTC &= ~(1<<PORTC5);
	
	if(multiplexacion == 0){
		display_anodo(numero);
		PORTB |= (1<<PORTB0);
	}
	else if(multiplexacion == 1){
		PORTD = ~(jg1 & 0x0F);
		PORTC |= (1<<PORTC4);
	}
	else{
		PORTD = ~(jg2 & 0x0F);
		PORTC |= (1<<PORTC5);
	}
	TCNT0 = 178; // se recarga el valor del tcnt0 para que vuelva a contar 5 ms.				
}