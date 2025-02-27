
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
#include "is31fl3732a.h"

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

// uint8_t led_states[12];

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

    is31fl3732a_init();

    while (1)
    {
        static uint8_t brightness = 0;
        tight_loop_contents();

        adc_select_input(0);
        adc_0 = adc_read();
        adc_select_input(1);
        adc_1 = adc_read();
        adc_select_input(2);
        adc_2 = adc_read();
        adc_select_input(3);
        adc_3 = adc_read();

        pos = calc_360degreePotPosition(adc_0, adc_1);
        pos2 = calc_360degreePotPosition(adc_2, adc_3);
        
        // single_led_in_section((pos / 57) + 72, 72, 144);
        // single_led_in_section((pos2 / 57), 0, 72);

        fill_led_range(pos, pos2, 0, 36);
        fill_led_range(pos, pos2, 37, 72);

        // set_brightness(pos2 / 16);
        // single_led(pos / 57);

        process_is31fl3732a();

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
