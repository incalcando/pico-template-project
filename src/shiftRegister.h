
#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "pico/stdlib.h"
#include "hardware/spi.h"


#define SPI_PORT_1 spi1
#define PIN_SCK_1 10
#define PIN_MOSI_1 11
#define PIN_CS_1 13

void set_bit_high(uint16_t *word, uint16_t n);
void set_bit_low(uint16_t *word, uint16_t n);
void flip_bit(uint16_t *word, uint16_t n);
void shift_register_init();
void send_shift_register_data(uint8_t data1, uint8_t data2);
void send_shift_register_data16(uint16_t data);

extern int adc_0;
extern int adc_1;
extern int adc_2;
extern int adc_3;

#endif