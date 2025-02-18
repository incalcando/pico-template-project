
#include "shiftRegister.h"

int adc_0;
int adc_1;
int adc_2;
int adc_3;

void set_bit_high(uint16_t *word, uint16_t n){
    *word |= (1 << n);
}
void set_bit_low(uint16_t *word, uint16_t n){
    *word &= ~(1 << n);
}
void flip_bit(uint16_t *word, uint16_t n){
    *word ^= (1 << n);
}

void shift_register_init() {
    // Initialize SPI1 at a clock speed of 1 MHz
    spi_init(SPI_PORT_1, 1000000);

    // Set the GPIO functions for the SPI pins
    gpio_set_function(PIN_SCK_1, GPIO_FUNC_SPI);  // SCK (Clock)
    gpio_set_function(PIN_MOSI_1, GPIO_FUNC_SPI); // MOSI (Master Out, Slave In)

    // Initialize Chip Select (CS) as a GPIO output
    gpio_init(PIN_CS_1);
    gpio_set_dir(PIN_CS_1, GPIO_OUT);
    gpio_put(PIN_CS_1, 1); // Set CS high initially (inactive state)
}

void send_shift_register_data(uint8_t data1, uint8_t data2) {
    gpio_put(PIN_CS_1, 0); // Pull CS low to start communication
    spi_write_blocking(SPI_PORT_1, &data1, 1); // Send the data byte
    spi_write_blocking(SPI_PORT_1, &data2, 1); // Send the data byte
    gpio_put(PIN_CS_1, 1); // Pull CS high to end communication
}

void send_shift_register_data16(uint16_t word) {
    gpio_put(PIN_CS_1, 0); // Pull CS low to start communication
    uint8_t data1 = word >> 8;
    uint8_t data2 = word & 0xFF;
    spi_write_blocking(SPI_PORT_1, &data1, 1); // Send the data byte
    spi_write_blocking(SPI_PORT_1, &data2, 1); // Send the data byte
    gpio_put(PIN_CS_1, 1); // Pull CS high to end communication
}
