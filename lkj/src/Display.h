
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
        void power_print(float);
        float find_power_level(uint8_t, uint64_t);
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
        uint64_t check_freq[CHECK_FREQ] = {25000000, 50000000, 75000000, 100000000, 150000000, 200000000, 250000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000, 1000000000, 1100000000, 1115000000, 1200000000, 1250000000, 1300000000, 1350000000, 1400000000, 1500000000, 1560000000, 1600000000, 1700000000, 1800000000, 1900000000, 2000000000, 2100000000, 2150000000, 2200000000, 2300000000, 2400000000, 2500000000, 2550000000, 2600000000, 2700000000, 2750000000, 2800000000, 2900000000};  
        uint8_t best_pow_level;
};


