
#pragma once
// #include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select

extern Adafruit_ST7789 tft;

class Display 
{
    public:
        void print_freq(uint64_t, uint8_t, uint8_t);
        void power_print(float);
        void my_print(uint64_t, uint16_t, uint16_t);
        void set_display();

    private:
        uint8_t get_number_of_characters(uint64_t);
    
        uint16_t freq_print_height = SCREEN_HEIGHT / 3;
        uint16_t power_print_height = SCREEN_HEIGHT / 2;
};


