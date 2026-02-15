



#ifndef HX711_H_
#define HX711_H_

#include <stdint.h>
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Pines (tu caso)
#ifndef HX711_PORT
#define HX711_PORT   PORTD
#endif
#ifndef HX711_PIN
#define HX711_PIN    PIND
#endif
#ifndef HX711_DDR
#define HX711_DDR    DDRD
#endif
#ifndef HX711_SCK
#define HX711_SCK    PD2
#endif
#ifndef HX711_DT
#define HX711_DT     PD3
#endif

typedef struct {
    int32_t offset;   // tare en cuentas
    float   scale;    // cuentas por gramo (o por tu unidad)
} HX711_Cal;

// --- SLAVE (lee sensor) ---
void    HX711_init(void);
uint8_t HX711_isReady(void);
int32_t HX711_readRaw24_A128(void);             // signed int32 (sign extend)
int32_t HX711_readAverage_A128(uint8_t samples);

// --- Calibración / tare (si lo quisieras hacer donde sea) ---
void    HX711_tare_fromRaw(HX711_Cal *cal, int32_t raw_avg);
void    HX711_setScale(HX711_Cal *cal, float counts_per_unit);
float   HX711_toUnits(const HX711_Cal *cal, int32_t raw);

// --- MASTER (decode desde 3 bytes enviados por el SLAVE) ---
int32_t HX711_decode24(uint8_t b2, uint8_t b1, uint8_t b0); // b2=MSB

#endif
