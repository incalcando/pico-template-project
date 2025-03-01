#ifndef WS2812_H
#define WS2821_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

RGB hsb_to_rgb(uint16_t hue, uint8_t sat, uint8_t bright) {
    RGB rgb;
    uint8_t region, remainder, p, q, t;

    if (sat == 0) {
        // If saturation is 0, the color is a shade of gray
        rgb.r = rgb.g = rgb.b = bright;
        return rgb;
    }

    // Scale hue from 0-360 to 0-5 (to find region)
    region = hue / 60;
    remainder = (hue % 60) * 255 / 60; // Scale remainder to 0-255

    // Calculate intermediate values
    p = (bright * (255 - sat)) / 255;
    q = (bright * (255 - ((sat * remainder) / 255))) / 255;
    t = (bright * (255 - ((sat * (255 - remainder)) / 255))) / 255;

    // Assign RGB values based on region
    switch (region) {
        case 0: rgb.r = bright; rgb.g = t; rgb.b = p; break;
        case 1: rgb.r = q; rgb.g = bright; rgb.b = p; break;
        case 2: rgb.r = p; rgb.g = bright; rgb.b = t; break;
        case 3: rgb.r = p; rgb.g = q; rgb.b = bright; break;
        case 4: rgb.r = t; rgb.g = p; rgb.b = bright; break;
        default: rgb.r = bright; rgb.g = p; rgb.b = q; break;
    }

    return rgb;
}

const uint8_t ws2812_colour[256][3] = {
    {0, 0, 0},
    {2, 1, 1},
    {3, 2, 2},
    {4, 3, 3},
    {5, 4, 4},
    {5, 5 ,5},
    {6, 6, 6},
    {7, 7, 7},
    {8, 8, 8},
    {9, 9, 9},
    {0, 0, 0},
    {11, 11, 11},
    {12, 12, 12},
    {13, 13, 13},
    {14, 14, 14},
    {15, 15 ,15},
    {16, 16, 16},
    {17, 17, 17},
    {18, 18, 18},
    {19, 19, 19},
    {20, 20, 20},
    {21, 21, 21},
    {22, 22, 22},
    {23, 23, 23},
    {24, 24, 24},
    {25, 25 ,25},
    {26, 26, 26},
    {27, 27, 27},
    {28, 28, 28},
    {29, 29, 29},
    {30, 30, 30},
    {31, 31, 31},
    {32, 32, 32},
    {33, 33, 33},
    {34, 34, 34},
    {35, 35 ,35},
    {36, 36, 36},
    {37, 37, 37},
    {38, 38, 38},
    {39, 39, 39},
    {40, 40, 40},
    {41, 41, 41},
    {42, 42, 42},
    {43, 43, 43},
    {44, 44, 44},
    {45, 45 ,45},
    {46, 46, 46},
    {47, 47, 47},
    {48, 48, 48},
    {49, 49, 49},
    {50, 50, 50},
    {51, 51, 51},
    {52, 52, 52},
    {53, 53, 53},
    {54, 54, 54},
    {55, 55 ,55},
    {56, 56, 56},
    {57, 57, 57},
    {58, 58, 58},
    {59, 59, 59},
    {60, 60, 60},
    {61, 61, 61},
    {62, 62, 62},
    {63, 63, 63},
    {64, 64, 64},
    {65, 65, 65},
    {66, 66, 66},
    {67, 67, 67},
    {68, 68, 68},
    {69, 69, 69},
    {70, 70, 70},
    {71, 71, 71},
    {72, 72, 72},
    {73, 73, 73},
    {74, 74, 74},
    {75, 75, 75},
    {76, 76, 76},
    {77, 77, 77},
    {78, 78, 78},
    {79, 79, 79},
    {80, 80, 80},
    {81, 81, 81},
    {82, 82, 82},
    {83, 83, 83},
    {84, 84, 84},
    {85, 85, 85},
    {86, 86, 86},
    {87, 87, 87},
    {88, 88, 88},
    {89, 89, 89},
    {90, 90, 90},
    {91, 91, 91},
    {92, 92, 92},
    {93, 93, 93},
    {94, 94, 94},
    {95, 95, 95},
    {96, 96, 96},
    {97, 97, 97},
    {98, 98, 98},
    {99, 99, 99},
    {100, 100, 100},
    {101, 101, 101},
    {102, 102, 102},
    {103, 103, 103},
    {104, 104, 104},
    {105, 105, 105},
    {106, 106, 106},
    {107, 107, 107},
    {108, 108, 108},
    {109, 109, 109},
    {110, 110, 110},
    {111, 111, 111},
    {112, 112, 112},
    {113, 113, 113},
    {114, 114, 114},
    {115, 115, 115},
    {116, 116, 116},
    {117, 117, 117},
    {118, 118, 118},
    {119, 119, 119},
    {120, 120, 120},
    {121, 121, 121},
    {122, 122, 122},
    {123, 123, 123},
    {124, 124, 124},
    {125, 125, 125},
    {126, 126, 126},
    {127, 127, 127},
    {128, 128, 128},
    {129, 129, 129},
    {130, 130, 130},
    {131, 131, 131},
    {132, 132, 132},
    {133, 133, 133},
    {134, 134, 134},
    {135, 135, 135},
    {136, 136, 136},
    {137, 137, 137},
    {138, 138, 138},
    {139, 139, 139},
    {140, 140, 140},
    {141, 141, 141},
    {142, 142, 142},
    {143, 143, 143},
    {144, 144, 144},
    {145, 145, 145},
    {146, 146, 146},
    {147, 147, 147},
    {148, 148, 148},
    {149, 149, 149},
    {150, 150, 150},
    {151, 151, 151},
    {152, 152, 152},
    {153, 153, 153},
    {154, 154, 154},
    {155, 155, 155},
    {156, 156, 156},
    {157, 157, 157},
    {158, 158, 158},
    {159, 159, 159},
    {160, 160, 160},
    {161, 161, 161},
    {162, 162, 162},
    {163, 163, 163},
    {164, 164, 164},
    {165, 165, 165},
    {166, 166, 166},
    {167, 167, 167},
    {168, 168, 168},
    {169, 169, 169},
    {170, 170, 170},
    {171, 171, 171},
    {172, 172, 172},
    {173, 173, 173},
    {174, 174, 174},
    {175, 175, 175},
    {176, 176, 176},
    {177, 177, 177},
    {178, 178, 178},
    {179, 179, 179},
    {180, 180, 180},
    {181, 181, 181},
    {182, 182, 182},
    {183, 183, 183},
    {184, 184, 184},
    {185, 185, 185},
    {186, 186, 186},
    {187, 187, 187},
    {188, 188, 188},
    {189, 189, 189},
    {190, 190, 190},
    {191, 191, 191},
    {192, 192, 192},
    {193, 193, 193},
    {194, 194, 194},
    {195, 195, 195},
    {196, 196, 196},
    {197, 197, 197},
    {198, 198, 198},
    {199, 199, 199},
    {200, 200, 200},
    {201, 201, 201},
    {202, 202, 202},
    {203, 203, 203},
    {204, 204, 204},
    {205, 205, 205},
    {206, 206, 206},
    {207, 207, 207},
    {208, 208, 208},
    {209, 209, 209},
    {210, 210, 210},
    {211, 211, 211},
    {212, 212, 212},
    {213, 213, 213},
    {214, 214, 214},
    {215, 215, 215},
    {216, 216, 216},
    {217, 217, 217},
    {218, 218, 218},
    {219, 219, 219},
    {220, 220, 220},
    {221, 221, 221},
    {222, 222, 222},
    {223, 223, 223},
    {224, 224, 224},
    {225, 225, 225},
    {226, 226, 226},
    {227, 227, 227},
    {228, 228, 228},
    {229, 229, 229},
    {230, 230, 230},
    {231, 231, 231},
    {232, 232, 232},
    {233, 233, 233},
    {234, 234, 234},
    {235, 235, 235},
    {236, 236, 236},
    {237, 237, 237},
    {238, 238, 238},
    {239, 239, 239},
    {240, 240, 240},
    {241, 241, 241},
    {242, 242, 242},
    {243, 243, 243},
    {244, 244, 244},
    {245, 245, 245},
    {246, 246, 246},
    {247, 247, 247},
    {248, 248, 248},
    {249, 249, 249},
    {250, 250, 250},
    {251, 251, 251},
    {252, 252, 252},
    {253, 253, 253},
    {254, 254, 254},
    {255, 255, 255}

};

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


void process_ws2812(uint8_t brightness1, uint8_t brightness3) {

    // if (!gpio_get(18))// && BTN_Flag0 == 0)
        // {

            // BTN_Flag0 = 1;  
            // channel++;
            // if (channel > 3)
            // {
            //     channel = 0;
            // }
        

            // hex code #B13000  //approx. Instruo Amber
            uint32_t amber = 0xB13000;
            uint32_t white = 0xC3C3C3;

            uint8_t amber_r = amber >> 16;
            uint8_t amber_g = amber >> 8;
            uint8_t amber_b = amber;

            uint8_t white_r = white >> 16;
            uint8_t white_g = white >> 8;
            uint8_t white_b = white;

            uint8_t adjusted_amber_r = (amber_r * ws2812_colour[brightness1][0]) >> 8;
            uint8_t adjusted_amber_g = (amber_g * ws2812_colour[brightness1][1]) >> 8;
            uint8_t adjusted_amber_b = (amber_b * ws2812_colour[brightness1][2]) >> 8;

            uint8_t adjusted_white_r = (white_r * brightness3) >> 8;
            uint8_t adjusted_white_g = (white_g * brightness3) >> 8;
            uint8_t adjusted_white_b = (white_b * brightness3) >> 8;

            // put_pixel(urgb_u32(adjusted_amber_r, adjusted_amber_g, adjusted_amber_b)); // button 1
            // put_pixel(urgb_u32(adjusted_white_r, adjusted_white_g, adjusted_white_b)); // button 2
            // put_pixel(urgb_u32(brightness1, brightness3, 0)); // button 1
            // put_pixel(urgb_u32(brightness1, brightness3, 0)); // button 2

            RGB amber_rgb = hsb_to_rgb(15, 255, brightness1);
            put_pixel(urgb_u32(amber_rgb.r, amber_rgb.g, amber_rgb.b)); // button 1
            RGB white_rgb = hsb_to_rgb(brightness3, 255, brightness1);
            put_pixel(urgb_u32(white_rgb.r, white_rgb.g, white_rgb.b)); // button 2

            // amber *= brightness1 >> 4;
            // white *= brightness3 >> 4;
            // hex code #C3C3C3  //approx. Instruo White
            // put_pixel(urgb_u32(0xB1, 0x30, 0x00)); // button 1
            // put_pixel(urgb_u32(0xC0, 0xC0, 0xC0)); // button 2
            // put_pixel(urgb_u32(amber_r, amber_g, amber_b)); // button 1
            // put_pixel(urgb_u32(white >> 16, white >> 8, white)); // button 2
            
            // outer RGB Ring
            for (int i = 0; i < 16; i++)
            {
                // put_pixel(urgb_u32(adjusted_amber_r, adjusted_amber_g, adjusted_amber_b));
                put_pixel(urgb_u32(amber_rgb.r, amber_rgb.g, amber_rgb.b));
            }
            for (int i = 0; i < 16; i++)
            {
                // put_pixel(urgb_u32(adjusted_white_r, adjusted_white_g, adjusted_white_b));
                put_pixel(urgb_u32(white_rgb.r, white_rgb.g, white_rgb.b));
            }

            


            // inner RGB Ring
            for (int i = 0; i < 16; i++)
            {
                // put_pixel(urgb_u32(adjusted_white_r, adjusted_white_g, adjusted_white_b));
                put_pixel(urgb_u32(white_rgb.r, white_rgb.g, white_rgb.b));
            }

            for (int i = 0; i < 16; i++)
            {
                // put_pixel(urgb_u32(adjusted_amber_r, adjusted_amber_g, adjusted_amber_b));
                put_pixel(urgb_u32(amber_rgb.r, amber_rgb.g, amber_rgb.b));
            }
            // sleep_ms(1);

}



#endif // WS2812_H



#