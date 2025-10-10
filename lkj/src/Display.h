
#pragma once
// #include "Arduino.h"
#include "power_table.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select


extern Adafruit_ST7789 tft;
//template <typename T>
class Display 
{
    public:
        void print_freq(uint64_t, uint8_t, uint8_t);
        void power_print(uint32_t);
        float find_power_level(uint8_t, uint64_t);
    
    protected:
        uint8_t get_best_level(uint8_t,  uint64_t);
    
    private:
        uint8_t get_number_of_characters(uint64_t);
       
        uint8_t closed_freq(uint64_t);
        uint8_t detect_best_left_level(uint8_t, uint64_t);
        uint8_t detect_best_right_level(uint8_t, uint64_t);
        float lt_power_diff(uint8_t, uint64_t);
        float rt_power_diff(uint8_t, uint64_t);
        struct freqs 
        {
            uint8_t left_freq_index;
            uint8_t right_freq_index;
        };
        freqs detect_index_of_side_freq(uint64_t);
        float my_map(float, float, float, float, float);
        //T my_map(T, T, T, T, T);                          //почему-то ломает freqs
    

        uint64_t min_diff = 0;
        uint64_t check_freq[CHECK_FREQ];  
        uint8_t best_pow_level;
};


