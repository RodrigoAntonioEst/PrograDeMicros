/*
 * HX711.c
 *
 * Created: 2/13/2026 6:26:25 PM
 *  Author: jargu
 */ 


#include "HX711.h"
#include <util/delay.h>

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

void HX711_init(void){
    HX711_DDR |= (1 << HX711_SCK);      // SCK salida PD2
    HX711_DDR &= ~(1 << HX711_DT);      // DT entrada PD3
    HX711_PORT &= ~(1 << HX711_SCK);    // SCK en 0
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

uint8_t HX711_isReady(void){
    return ((HX711_PIN & (1 << HX711_DT)) == 0);    // Esperar a que los datos estén listos para leerse
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------


// Lee 24 bits + 1 pulso extra (A, ganancia 128)
int32_t HX711_readRaw24_A128(void){
    uint32_t data = 0;

    while(!HX711_isReady()); // DT=0 cuando hay dato listo

    for(uint8_t i = 0; i < 24; i++){
        HX711_PORT |= (1 << HX711_SCK);
        _delay_us(1);

        data <<= 1;
        if(HX711_PIN & (1 << HX711_DT)) data |= 1;

        HX711_PORT &= ~(1 << HX711_SCK);
        _delay_us(1);
    }

    // 1 pulso extra -> selecciona A128 para la próxima conversión
    HX711_PORT |= (1 << HX711_SCK);
    _delay_us(1);
    HX711_PORT &= ~(1 << HX711_SCK);
    _delay_us(1);

    // sign extend 24->32
    if(data & 0x800000UL) data |= 0xFF000000UL;
    return (int32_t)data;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

int32_t HX711_readAverage_A128(uint8_t samples){
    if(samples == 0) samples = 1;
    int64_t sum = 0;
    for(uint8_t i=0;i<samples;i++){
        sum += HX711_readRaw24_A128();
    }
    return (int32_t)(sum / samples);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

int32_t HX711_decode24(uint8_t b2, uint8_t b1, uint8_t b0){
    uint32_t data = ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b0;
    if(data & 0x800000UL) data |= 0xFF000000UL;
    return (int32_t)data;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

// Calibración/tare “desde raw” (para que sea usable en MASTER)
void HX711_tare_fromRaw(HX711_Cal *cal, int32_t raw_avg){
    if(!cal) return;
    cal->offset = raw_avg;
    if(cal->scale == 0.0f) cal->scale = 1.0f;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

void HX711_setScale(HX711_Cal *cal, float counts_per_unit){
    if(!cal) return;
    if(counts_per_unit == 0.0f) counts_per_unit = 1.0f;
    cal->scale = counts_per_unit;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

float HX711_toUnits(const HX711_Cal *cal, int32_t raw){
    if(!cal) return 0.0f;
    return (float)(raw - cal->offset) / cal->scale;
}
