#ifndef IS31FL3732A_H
#define IS31FL3732A_H

#include <stdio.h>

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C port to control the LED driver chip IS31FL3732A
#define I2C_PORT i2c0
#define PIN_SDA 16
#define PIN_SCL 17
#define PIN_SDB 25
#define PIN_INTB 24

#define MAX_10BIT 1023
#define MAX_12BIT 4095
#define MAX_16BIT 65535

#define IS31FL3732A_ADDR 0x50  // 0101 0000

// uint8_t led_states[0x11] = {0};
uint8_t led_brightness[0x90] = {0};

uint8_t brightness = 0xFF;


int pos;
int pos2;

void set_brightness(uint8_t b){
    brightness = b;
}

int led_matrix[0x90] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    16, 17, 18, 19, 20, 21, 22, 23,
    32, 33, 34, 35, 36, 37, 38, 39,
    48, 49, 50, 51, 52, 53, 54, 55,
    64, 65, 66, 67, 68, 69, 70, 71,
    80, 81, 82, 83, 84, 85, 86, 87,
    96, 97, 98, 99, 100, 101, 102, 103,
    112, 113, 114, 115, 116, 117, 118, 119,
    128, 129, 130, 131, 132, 133, 134, 135,
    8, 9, 10, 11, 12, 13, 14, 15,
    24, 25, 26, 27, 28, 29, 30, 31,
    40, 41, 42, 43, 44, 45, 46, 47,
    56, 57, 58, 59, 60, 61, 62, 63,
    72, 73, 74, 75, 76, 77, 78, 79,
    88, 89, 90, 91, 92, 93, 94, 95,
    104, 105, 106, 107, 108, 109, 110, 111,
    120, 121, 122, 123, 124, 125, 126, 127,
    136, 137, 138, 139, 140, 141, 142, 143
};

int led_matrix_hex[0x90] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F
};


void i2c_write_register(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(I2C_PORT, IS31FL3732A_ADDR, buf, 2, false);
};

void is31fl3732a_init() {
    i2c_write_register(0xFD, 0x0B);  //soft
    i2c_write_register(0x0A, 0x00);

    i2c_write_register(0xFD, 0x00);  // Select Page 0 (LED Control Registers)

    for (uint8_t reg = 0x00; reg < 0x12; reg++) {
        i2c_write_register(reg, 0b11111111);
    } 
    for (uint8_t reg = 0x12; reg <= 0x24; reg++) {
        i2c_write_register(reg, 0x00);  // disable blinking
    }

    for (uint8_t led = 0x24; led <= 0xB3; led++) {
        i2c_write_register(led, 0xFF);  // Set all LEDs to full brightness (PWM = 255)
        i2c_write_register(led, 0x00);  // Set all LEDs to lowest brightness (PWM = 0)
    }

    i2c_write_register(0xFD, 0x0B);  // Select Page 9 (Global Current Control)
    i2c_write_register(0x00, 0x00);  // set to picture mode 
    i2c_write_register(0x01, 0x00);  // set picture to frame 1
    i2c_write_register(0x02, 0x00);  // auto play to endless (necesary ?)
    i2c_write_register(0x04, 0xFF);  // Set global current to 0xFF (maximum current/brightness)
    i2c_write_register(0x05, 0b00001100);  // Set global blink control
    i2c_write_register(0x09, 0x00);  // Set breath control to active in picture mode (needed?)
    i2c_write_register(0x0A, 0x01);  // Put display driver to Normal Operation Mode from Softare Shutdown Mode 1
    i2c_write_register(0x0B, 0x00);  // disable AGC (needed?)
    i2c_write_register(0xFD, 0x00);  // go back to page 1 frame register 1 
};

void send_all_brightness(){
    for (uint8_t i = 0x00; i < 0x90; i++) {
        i2c_write_register(0x24 + led_matrix_hex[i], led_brightness[i]);
    }
};


void single_led(int pos){
    if(pos < 0x90)
            led_brightness[pos] = brightness;
}


void single_led_in_section(int pos, int start, int end){
    // static uint8_t reg = 0x00;
    if(pos >= start && pos <= end)
    {
        for(int i = start; i < end; i++){
            if(i == pos)
                led_brightness[i] = brightness;
            else
                led_brightness[i] = 0x00;
        }
    }
    
}


void control_led_range_ring_buffer(int start, int length, int first, int last) {
    int rangeLED = last - first + 1;

    for (int i = 0; i < rangeLED; i++) {
        int pos = (start + i) % rangeLED;
        if (i < length) {
            led_brightness[first + pos] = brightness;
        } else {
            led_brightness[first + pos] = 0x00;
        }
    }
}

void fill_led_range(float start, float length, float first, float last) {
    
    ////// This is the new code
    float overspill = 0;
    float rangeLED = 0;
    float startOfLEDS = start;
    float amountOfLEDS = length;

    rangeLED = last - first + 1;
    startOfLEDS = first + ((startOfLEDS * rangeLED) / MAX_12BIT);
    amountOfLEDS = (amountOfLEDS * rangeLED) / MAX_12BIT;

    control_led_range_ring_buffer((int)startOfLEDS, (int)amountOfLEDS, (int)first, (int)last);

}


float calc_360degreePotPosition(uint16_t adc1, uint16_t adc2)
{
    int16_t x = adc1 - (MAX_12BIT / 2);
    int16_t y = adc2 - (MAX_12BIT    / 2);
    //compute the angle in radians
    float angle = (atan2(y, x) + M_PI);
    // convert to 0-4095 range
    float position = fmod((((angle + M_PI) / (2 * M_PI)) + 0.f), 1.f) * MAX_12BIT;
    return position;
}

// void control_led_range(int start, int end, int brightness){
//     // for(int i = start; i < end; i++){
//     //     led_brightness[i] = brightness;
//     // }

//     int overspill = 0;
//     int startOfLEDS = (calc_360degreePotPosition(adc_0, adc_1));
//     int amountOfLEDS = (calc_360degreePotPosition(adc_2, adc_3));

//     if(startOfLEDS + amountOfLEDS > MAX_12BIT)
//         overspill = (startOfLEDS + amountOfLEDS) - MAX_12BIT;
//     for (int i = 0; i < overspill; i++)
//         set_led_state(i, true, 0);
//     for (int i = overspill; i < startOfLEDS; i++)
//         set_led_state(i, false, 0);
//     for (int i = startOfLEDS; i < startOfLEDS+amountOfLEDS; i++)
//         if (i < MAX_12BIT) set_led_state(i, true, 0);
//     for (int i = startOfLEDS+amountOfLEDS; i < MAX_12BIT; i++)
//         if (i < MAX_12BIT) set_led_state(i, false, 0);

//     for(int i = 0; i < MAX_12BIT; i++){
//         if (i > calc_360degreePotPosition(adc_0, adc_1 && i < calc_360degreePotPosition(adc_2, adc_3)))
//             set_led_state(i, true, 0);
//         else
//             set_led_state(i, false, 0);
//     }
// }

uint8_t led_brighness_pulse(){
    static int br = 0;
    static int dir = 2;
    br += dir;
    if(br > 255) {dir = -2; br = 255;}
    else if (br < 0) {dir = 1; br = 0;}
    return br;
}

void process_is31fl3732a()
{
    static int frame = 0;
    if(frame > 5){
        // set_brightness(led_brighness_pulse());
        set_brightness(0xFF);
        send_all_brightness();
        frame = 0;
    }
    frame++;

}

#endif // IS31FL3732A_H