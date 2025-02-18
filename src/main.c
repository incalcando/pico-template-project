
#include <stdio.h>
#include "pico/stdlib.h"
#include "log.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/gpio.h" // ?? needed??
#include "hardware/i2c.h"
#include "ws2812.pio.h"
#include "tusb.h"

#include "math.h"

#include "shiftRegister.h"

// SPI port 0
#define SPI_PORT spi0
#define PIN_SCK 2
#define PIN_MOSI 3
#define PIN_CS 5

// SPI port 1
#define SPI_PORT_1 spi1
#define PIN_SCK_1 10
#define PIN_MOSI_1 11
#define PIN_CS_1 13

// I2C port to control the LED driver chip IS31FL3732A
#define I2C_PORT i2c0
#define PIN_SDA 16
#define PIN_SCL 17
#define PIN_SDB 25
#define PIN_INTB 24

#define MAX_10BIT 1023
#define MAX_12BIT 4095
#define MAX_16BIT 65535

// #define IS31FL3732A_ADDR 0x3E
// #define IS31FL3732A_ADDR 0xA0 // 1010 0000
#define IS31FL3732A_ADDR 0x50  // 0101 0000

int led_matrix[144] = {
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

int led_matrix_hex[144] = {
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


// // write a byte to the IS31FL3732A
// int is31fl3732a_write(uint8_t reg, uint8_t data)
// {
//     uint8_t buf[2] = {reg, data};
//     return i2c_write_blocking(I2C_PORT, IS31FL3732A_ADDR, buf, 2, false);
// }

// Function to write to IS31FL3732A register via I2C
void i2c_write_register(uint8_t reg, uint8_t data) {
    // Implement your I2C write function here.
    // Example: i2c_write(IS31FL3732A_ADDR, reg, data);
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(I2C_PORT, IS31FL3732A_ADDR, buf, 2, false);
}

// Function to initialize the IS31FL3732A
void is31fl3732a_init() {
    // uint8_t buf[2] = {100, 100};

    // volatile bool success = false;
    // success = i2c_write_blocking(I2C_PORT, IS31FL3732A_ADDR, buf, 2, false);
    // printf("I2C write success: %d\n", success);

    // i2c_write_register(0x00, 0x00);  // Enable normal operation (not shutdown)
    i2c_write_register(0xFD, 0x0B);  //soft
    i2c_write_register(0x0A, 0x00);

    i2c_write_register(0xFD, 0x00);  // Select Page 0 (LED Control Registers)
    // i2c_write_register(0xFE, 0xC5);  // Unlock protected registers

    for (uint8_t reg = 0x00; reg < 0x12; reg++) {
        // if(reg == 0x00) i2c_write_register(reg, 0b00000001);  // Enable all LEDs (each bit corresponds to an LED)
        // else if(reg == 0x01) i2c_write_register(reg, 0b1000000); 
        // else i2c_write_register(reg, 0b00000000);  // Enable all LEDs (each bit corresponds to an LED)
        i2c_write_register(reg, 0b00000000);
        // i2c_write_register(reg, 0b00000001);  // Enable all LEDs (each bit corresponds to an LED)
    } 
    for (uint8_t reg = 0x12; reg <= 0x24; reg++) {
        // uint8_t rand = (uint8_t)random();
        i2c_write_register(reg, 0x00);  // disable blinking
        // i2c_write_register(reg, 0xFF);  // enable blinking
        // i2c_write_register(reg, rand);  // disable blinking
    }

    for (uint8_t led = 0x24; led <= 0xB3; led++) {
        i2c_write_register(led, 0xFF);  // Set all LEDs to full brightness (PWM = 255)
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

}




// void is31fl3732a_init()
// {
//     gpio_init(PIN_SDB);
//     gpio_set_dir(PIN_SDB, GPIO_OUT);
//     gpio_put(PIN_SDB, 1); // Set SDB high to enable the chip

//     gpio_init(PIN_INTB);
//     gpio_set_dir(PIN_INTB, GPIO_IN);
//     gpio_pull_up(PIN_INTB); // Enable pull-up resistor on INTB

//     // // Unlock command register
//     // if (is31fl3732a_write(0xFE, 0xC5) != 2) {
//     //     printf("Failed to unlock command register\n");
//     // }

//     // Unlock Command register
//     is31fl3732a_write(0xFD, 0x00);

//     // // Set operation mode to normal operation mode
//     // if (is31fl3732a_write(0x0A, 0x01) != 2) {
//     //     printf("Failed to set operation mode\n");
//     // }

//     // Enable all LED channels
//     // for (uint8_t page = 0; page < 8; page++) {
//     //     if (is31fl3732a_write(0xFD, page) != 2) {
//     //         printf("Failed to select page %d\n", page);
//     //     }
//         for (uint8_t reg = 0x00; reg <= 0x11; reg++) {
//             is31fl3732a_write(reg, 0xFF);
//         }
//     // }

//     // // Set global current control
//     // if (is31fl3732a_write(0xFD, 0x0B) != 2) {
//     //     printf("Failed to select function page\n");
//     // }
//     // if (is31fl3732a_write(0x01, 0xFF) != 2) {
//     //     printf("Failed to set global current\n");
//     // }
// }

// void set_white_led_brightness(uint8_t led, uint8_t brightness)
// {
//     // uint8_t reg = 0x24 + led;
//     uint8_t reg = led;
//     // if (is31fl3732a_write(reg, brightness) != 2) {
//         is31fl3732a_write(reg, brightness);
//         // printf("Failed to set brightness for LED %d\n", led);
//     // }
// }

// #define dac_ch0 16 //beta 8
// #define dac_ch1 8 // 
// #define dac_ch2 15 
// #define dac_ch3 7

// #define dac_ch4 14
// #define dac_ch5 6
// #define dac_ch6 13
// #define dac_ch7 5

// #define dac_ch8 12
// #define dac_ch9 4
// #define dac_ch10 11
// #define dac_ch11 3

// #define dac_ch12 10
// #define dac_ch13 2
// #define dac_ch14 9
// #define dac_ch15 1




/*

#ifdef PICO_DEFAULT_LED_PIN
void on_pwm_wrap()
{
    static int fade = 0;
    static bool going_up = true;
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN));

    if (going_up)
    {
        ++fade;
        if (fade > 255)
        {
            fade = 255;
            going_up = false;
        }
    }
    else
    {
        --fade;
        if (fade < 0)
        {
            fade = 0;
            going_up = true;
        }
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, fade * fade);
}
#endif
*/

#define IS_RGBW false
#define NUM_PIXELS 66

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 20
#endif

static uint16_t SR_data = 0;

uint8_t led_states[12];

void dac70508_init() {
    // Initialize SPI at 1 MHz
    spi_init(SPI_PORT, 400000);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Initialize Chip Select (CS) pin
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // CS is active low, so set it high initially
}

void dac70508_write(uint8_t channel, uint16_t value) {
    // The first byte contains the command and address (channel).
    // Command: 0x3 = Write to DAC Register
    uint8_t command = 0x30 | (channel & 0x0F); // 0x30 is the command for writing to DAC

    // Break the 24-bit frame into three 8-bit segments
    uint8_t buf[3];
    buf[0] = command;
    buf[1] = (value >> 8) & 0xFF; // MSB of the value
    buf[2] = value & 0xFF;        // LSB of the value

    gpio_put(PIN_CS, 0); // Pull CS low to start communication
    spi_write_blocking(SPI_PORT, buf, 3);
    gpio_put(PIN_CS, 1); // Pull CS high to end communication
}

void dac70508_set_all_channels(uint16_t value) {
    uint8_t combined_buf[48]; // 8 channels * 3 bytes/frame * 2 DACs = 24 bytes

    // Fill frames for the first DAC
    for (int channel = 0; channel < 8; channel++) {
        uint8_t command = 0x30 | (channel & 0x0F); // 0x3 is the command for "write to DAC register"
        combined_buf[channel * 3] = command;
        combined_buf[channel * 3 + 1] = (value >> 8) & 0xFF; // MSB
        combined_buf[channel * 3 + 2] = value & 0xFF;        // LSB
    }

    // Fill frames for the second DAC
    for (int channel = 0; channel < 8; channel++) {
        uint8_t command = 0x30 | (channel & 0x0F); // Same command for each channel
        combined_buf[(channel + 8) * 3] = command;
        combined_buf[(channel + 8) * 3 + 1] = (value >> 8) & 0xFF; // MSB
        combined_buf[(channel + 8) * 3 + 2] = value & 0xFF;        // LSB
    }

    gpio_put(PIN_CS, 0); // Pull CS low to start communication
    spi_write_blocking(SPI_PORT, combined_buf, 48); // Write all frames
    gpio_put(PIN_CS, 1); // Pull CS high to end communication
}

void dac70508_write_channel_first_dac(uint8_t channel, uint16_t value) {
    uint8_t command = 0x30 | (channel & 0x0F);
    uint8_t buf[3];
    buf[0] = command;
    buf[1] = (value >> 8) & 0xFF; // MSB
    buf[2] = value & 0xFF;        // LSB

    gpio_put(PIN_CS, 0); // Pull CS low to start communication
    spi_write_blocking(SPI_PORT, buf, 3); // Write the single frame
    gpio_put(PIN_CS, 1); // Pull CS high to end communication
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           ((uint32_t)(w) << 24) |
           (uint32_t)(b);
}

void pattern_snakes(uint len, uint t)
{
    for (uint i = 0; i < len; ++i)
    {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(urgb_u32(0, 0, 0xff));
        else
            put_pixel(0);
    }
}

void pattern_random(uint len, uint t)
{
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(rand());
}

void pattern_sparkle(uint len, uint t)
{
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(uint len, uint t)
{
    uint max = 100; // let's not draw too much current!
    t %= max;
    for (uint i = 0; i < len; ++i)
    {
        put_pixel(t * 0x10101);
        if (++t >= max)
            t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct
{
    pattern pat;
    const char *name;
} pattern_table[] = {
    {pattern_snakes, "Snakes!"},
    {pattern_random, "Random data"},
    {pattern_sparkle, "Sparkles"},
    {pattern_greys, "Greys"},
};

bool timer_callback_SR(repeating_timer_t *rt) {
    // Code to execute when timer triggers
    static uint16_t setBit = 0;
    flip_bit(&SR_data, setBit);
    setBit++;
    if(setBit > 16) setBit = 0;
    send_shift_register_data16(SR_data);
    return true; // Return true to repeat, false to stop
}

bool timer_callback_LED(repeating_timer_t *rt) {

    ///// UNCOMMENT TO SWITCH RGB TEST ON`
    // adc_select_input(0);
    //         uint8_t brightness = (adc_read() >> 4);
    //         adc_select_input(1);
    //         uint8_t brightness2 = (adc_read() >> 4);
            
            
    //         adc_select_input(2);
    //         uint8_t brightness3 = (adc_read() >> 4);

    //         adc_select_input(3);
    //         uint8_t brightness4 = (adc_read() >> 4);

    //         put_pixel(urgb_u32(brightness, brightness3, brightness4));
    //         put_pixel(urgb_u32(brightness3, brightness, brightness2));
            
    //         for (int i = 0; i < 16; i++)
    //         {
    //             put_pixel(urgb_u32(brightness, brightness3, brightness4));
    //         }
    //         for (int i = 0; i < 16; i++)
    //         {
    //             put_pixel(urgb_u32(brightness3, brightness, brightness2));
    //         }

    //         for (int i = 0; i < 16; i++)
    //         {
    //             put_pixel(urgb_u32(brightness4, 0, brightness4));
    //         }

    //         for (int i = 0; i < 16; i++)
    //         {
    //             put_pixel(urgb_u32(brightness4, brightness4, brightness4));
    //         }

    return true;
}

void setup_hardware_timer() {
    repeating_timer_t timer_SR;
    // repeating_timer_t timer_LED;
    // Set a repeating timer with a 500 ms interval
    add_repeating_timer_ms(62.5, timer_callback_SR, NULL, &timer_SR);
    // add_repeating_timer_ms(5, timer_callback_LED, NULL, &timer_LED);
}

void single_led_in_matrix(int pos, int matrix){
    static uint8_t reg = 0x00;
    static int data;
    data = pos / 57;
    static int b = 0;
    static uint8_t pReg;
    b = 1 << data % 8;
    reg = 0x00 + (data / 8) * 2;
    reg += matrix;
    i2c_write_register(reg, b);
    if(pReg != reg)
        i2c_write_register(pReg, 0x00);
    if (data > 71) data = 0;
    pReg = reg;
    if(reg > 0x11) reg = 0x00;
}

void set_led_state(int led_pos, bool state, uint8_t matrix) {
    uint8_t reg = 0x00;
    uint8_t data;
    static uint8_t pData = 0;
    data = led_pos / 57;
    uint8_t bit_pos = data % 8;
    uint8_t byte_pos = data / 8;

    if(pData != data){
        pData = data;

        // Calculate the register address
        reg = 0x00 + (byte_pos * 2);
        // reg = reg > 0x11 ? reg - 0x12 : reg;
        reg += matrix;

        // Write the register address to the device
        i2c_write_blocking(I2C_PORT, IS31FL3732A_ADDR, &reg, 1, true);

        // Read the current value of the register
        // uint8_t current_value = 0; //led_states[byte_pos];
        // i2c_read_blocking(I2C_PORT, IS31FL3732A_ADDR, &reg, 1, &current_value, 1, false);
        // i2c_read_blocking(I2C_PORT, IS31FL3732A_ADDR, &current_value, 1, false);

        // Set or clear the bit
        if (state) {
            led_states[byte_pos] |= (1 << bit_pos);
        } else {
            led_states[byte_pos] &= ~(1 << bit_pos);
        }

        // Write the updated value back to the register
        i2c_write_register(reg, led_states[byte_pos]);
    }
}

void range_led_in_matrix(int pos, int length, int matrix) {
    static uint8_t reg = 0x00;
    static int data;
    static uint8_t b = 0;
    static uint8_t pReg;

    for (int i = 0; i < length; i++) {
        data = (pos + i) / 8;
        b |= 1 << ((pos + i) % 8);
        reg = 0x00 + (data / 8) * 2;
        reg += matrix;
        i2c_write_register(reg, b);
        if (pReg != reg)
            i2c_write_register(pReg, 0x00);
        pReg = reg;
    }
}

void set_led_pwm(uint8_t led, uint8_t pwm) {
    // Set the PWM value for the specified LED
    int led_mult = (led / 8);
    int led_matrix = led_mult % 2;
    if(led < 0xB3)
        i2c_write_register(0x24 + led_matrix_hex[led], pwm);
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

void process_is31fl3732a()
{

    // calc_360degreePotPosition(adc_0, adc_1);
    single_led_in_matrix(calc_360degreePotPosition(adc_0, adc_1), 0);
    single_led_in_matrix(calc_360degreePotPosition(adc_2, adc_3), 1);

    // set_led_state(calc_360degreePotPosition(adc_0, adc_1), true, 0);
    // set_led_state(calc_360degreePotPosition(adc_2, adc_3), false, 0);
    static int frame = 0;
    static uint8_t pwm = 0;
    static int8_t direction = 1;
    if(frame > 5){
        set_led_pwm(72, pwm += direction);
        for(int i = 8; i < 15; i++){
            set_led_pwm(i, pwm);
        }
        for(int i = 80; i < 95; i++){
            set_led_pwm(i, 255 - pwm);
        }
        set_led_pwm(0, 255 - pwm);
        if(pwm == 255){
            direction = -1;
        } else if (pwm == 0){
            direction = 1;
        }
        frame = 0;
    }
    frame++;
    

    // int overspill = 0;
    // int startOfLEDS = (calc_360degreePotPosition(adc_0, adc_1));
    // int amountOfLEDS = (calc_360degreePotPosition(adc_2, adc_3));

    // if(startOfLEDS + amountOfLEDS > MAX_12BIT)
    //     overspill = (startOfLEDS + amountOfLEDS) - MAX_12BIT;
    // for (int i = 0; i < overspill; i++)
    //     set_led_state(i, true, 0);
    // for (int i = overspill; i < startOfLEDS; i++)
    //     set_led_state(i, false, 0);
    // for (int i = startOfLEDS; i < startOfLEDS+amountOfLEDS; i++)
    //     if (i < MAX_12BIT) set_led_state(i, true, 0);
    // for (int i = startOfLEDS+amountOfLEDS; i < MAX_12BIT; i++)
    //     if (i < MAX_12BIT) set_led_state(i, false, 0);

    // for(int i = 0; i < MAX_12BIT; i++){
    //     if (i > calc_360degreePotPosition(adc_0, adc_1 && i < calc_360degreePotPosition(adc_2, adc_3)))
    //         set_led_state(i, true, 0);
    //     else
    //         set_led_state(i, false, 0);
    // }
}

int main()
{
    stdio_init_all();
    tusb_init();
    dac70508_init();
    shift_register_init();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    adc_gpio_init(29);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
    // set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    i2c_init(I2C_PORT, 1000000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    gpio_init(PIN_SDB);
    gpio_set_dir(PIN_SDB, GPIO_OUT);
    gpio_put(PIN_SDB, 1); // Set SDB high to enable the chip

    gpio_init(PIN_INTB);
    gpio_set_dir(PIN_INTB, GPIO_IN);
    gpio_pull_up(PIN_INTB); 

    is31fl3732a_init();

    repeating_timer_t timer_SR;
    repeating_timer_t timer_LED;
    // Set up a repeating timer with a 500 ms interval
    bool result = add_repeating_timer_ms(62.5, timer_callback_SR, NULL, &timer_SR);
    bool result_LED = add_repeating_timer_ms(25, timer_callback_LED, NULL, &timer_LED);
    // setup_hardware_timer();



    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;

    gpio_init(18);
    gpio_set_dir(18, GPIO_IN);
    gpio_init(19);
    gpio_set_dir(19, GPIO_IN);
    uint8_t BTN_Flag0 = 0;
    uint8_t channel = 0;
    
    // set_bit_high(&SR_data, 0);
    // set_bit_low(&SR_data, 1);

    // for (uint8_t page = 0; page < 8; page++) {
    //     if (is31fl3732a_write(0xFD, page) != 2) {
    //         printf("Failed to select page %d\n", page);
    //     }
    //     for (uint8_t reg = 0x24; reg <= 0xB3; reg++) {
    //         if (is31fl3732a_write(reg, 0xFF) != 2) {
    //             printf("Failed to set brightness for LED at reg %d on page %d\n", reg, page);
    //         }
    //     }
    // }

    // is31fl3732a_init();     // Initialize LED driver
    // enable_leds();          // Enable first 8 LEDs
    // set_led_brightness();   // Set first 8 LEDs to full brightness
    
    for (uint8_t led = 16; led < 32; led++) { // max leds 144
        set_led_pwm(led, 32);  // Set all LEDs to full brightness (PWM = 255)
    }

    while (1)
    {

        tight_loop_contents();

            adc_select_input(0);
            adc_0 = adc_read();
            adc_select_input(1);
            adc_1 = adc_read();
            adc_select_input(2);
            adc_2 = adc_read();
            adc_select_input(3);
            adc_3 = adc_read();

        static int frame = 0;
        if(frame > 10){
            process_is31fl3732a();
            frame = 0;
        }
        frame++;

        // if (!gpio_get(18))// && BTN_Flag0 == 0)
        // {

            // BTN_Flag0 = 1;
            // channel++;
            // if (channel > 3)
            // {
            //     channel = 0;
            // }
            // adc_select_input(0);
            // brightness = (adc_read() >> 4);
            // adc_select_input(1);
            // uint8_t brightness2 = (adc_read() >> 4);
            // put_pixel(urgb_u32(0, brightness, brightness));
            // put_pixel(urgb_u32(brightness2, 0, brightness2));
            
            // adc_select_input(2);
            // uint8_t brightness3 = (adc_read() >> 4);
            
            // for (int i = 0; i < 16; i++)
            // {
            //     put_pixel(urgb_u32(brightness3, brightness3, 0));
            // }
            // for (int i = 0; i < 16; i++)
            // {
            //     put_pixel(urgb_u32(0, brightness3, brightness3));
            // }

            // adc_select_input(3);
            // uint8_t brightness4 = (adc_read() >> 4);

            // for (int i = 0; i < 16; i++)
            // {
            //     put_pixel(urgb_u32(brightness4, 0, brightness4));
            // }

            // for (int i = 0; i < 16; i++)
            // {
            //     put_pixel(urgb_u32(brightness4, brightness4, brightness4));
            // }
            // sleep_ms(1);

            // alarm_callback_t
        // }
        // if (gpio_get(18))// && BTN_Flag0 == 1)
        // {
        //     BTN_Flag0 = 0;
        //     put_pixel(urgb_u32(0, 0, 0));
        //     put_pixel(urgb_u32(0, 0, 0));
        //     for (int i = 0; i < 64; i++)
        //     {
        //         put_pixel(urgb_u32(0, 0, 0));
        //     }
        // }

        // put_pixel(rand() % 16 ? 0 : 0xffffffff);
        // put_pixel(rand() % 16 ? 0 : 0xffffffff);
        // sleep_ms(10);

        // int pat = rand() % count_of(pattern_table);
        // int dir = (rand() >> 30) & 1 ? 1 : -1;
        // puts(pattern_table[pat].name);
        // puts(dir == 1 ? "(forward)" : "(backward)");
        // for (int i = 0; i < 1000; ++i)
        // {
        //     pattern_table[pat].pat(NUM_PIXELS, t);
        //     sleep_ms(10);
        //     t += dir;
        // }

        // dac70508_set_all_channels(0x8000);
        // dac70508_write_channel_first_dac(0, 0x2356);
        // send_shift_register_data(0b10000000, 0b10000001 ); // First byte for second dac ()
        // static int setBit = 0;
        // flip_bit(&SR_data, setBit);
        // setBit++;
        // if(setBit > 16) setBit = 0;
        // send_shift_register_data16(SR_data);
        // sleep_ms(125); // Wait for 1 second

    }
}

/*
int main()
{
    stdio_init_all();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    adc_gpio_init(29);
    // Select ADC input 0 (GPIO26)
    // adc_select_input(0);

    log_open();

    gpio_set_function(25, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(25);
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);

    gpio_init(18);
    gpio_set_dir(18, GPIO_IN);
    gpio_init(19);
    gpio_set_dir(19, GPIO_IN);
    uint8_t BTN_Flag0 = 0;
    uint8_t channel = 0;
    while (1)
    {

        if (!gpio_get(18) && BTN_Flag0 == 0)
        {
            BTN_Flag0 = 1;
            channel++;
            if (channel > 3)
            {
                channel = 0;
            }
        }
        if (gpio_get(18) && BTN_Flag0 == 1)
        {
            BTN_Flag0 = 0;
        }

        adc_select_input(channel);
        uint16_t result = adc_read();

        // bright++;

        pwm_set_gpio_level(25, result << 4);
        LOG_INFO(result);
        // sleep_ms(1);

    }

    return 0;
}

*/
/*

int main() {
#ifndef PICO_DEFAULT_LED_PIN
#warning pwm/led_fade example requires a board with a regular LED
#else
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

    // Everything after this point happens in the PWM interrupt handler, so we
    // can twiddle our thumbs
    while (1)
        tight_loop_contents();
#endif
}
*/