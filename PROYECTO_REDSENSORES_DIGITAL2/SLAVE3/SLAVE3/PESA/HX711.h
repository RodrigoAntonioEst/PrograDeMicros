#ifndef HX711_H
#define HX711_H

#include <avr/io.h>
#include <stdint.h>

// ================= PINES (CAMBIA SI QUIERES) =================
// DOUT = PD2
// CLK  = PD3

#define HX711_DOUT_PIN   PD2
#define HX711_CLK_PIN    PD3

#define HX711_DOUT_PORT  PORTD
#define HX711_DOUT_DDR   DDRD
#define HX711_DOUT_PINR  PIND

#define HX711_CLK_PORT   PORTD
#define HX711_CLK_DDR    DDRD

// ================= FUNCIONES =================
void HX711_Init(void);
long HX711_ReadRaw(void);

void HX711_Tare(void);
void HX711_SetScale(float known_weight);
float HX711_GetWeight(void);

// ================= VARIABLES =================
extern long hx711_offset;
extern float hx711_scale;

#endif