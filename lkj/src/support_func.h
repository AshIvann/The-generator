#include <Arduino.h>
#include "stdint.h"
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include "GyverEncoder.h"
#include "Keypad.h"
#include <math.h>
// #include <avr/pgmspace.h>

//Пины для дислпея 
#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select

//Для энкодера 
#define CLK 2
#define DT 3
#define SW 4

#define PLL_DEN 10e6;
#define FREQ_OF_PHASE_DETECTOR 10;

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 360 

extern Adafruit_ST7789 tft;
extern Encoder enc1;
extern Keypad keypad ;

extern unsigned int divider_values[18];

extern uint64_t freq_set_by_encoder;            //выбор частоты для second_freq
extern uint32_t first_five;
extern uint32_t last_six;

struct st_freq_params{
  uint32_t first_five_of_freq;
  uint32_t last_six_of_freq;
  uint8_t chdiv;
  uint32_t int_part_of_frac_div;
};






int find_chdiv(uint64_t);                                            //определение значения делителя
uint16_t replace_bits_8_to_13(uint16_t, uint8_t);
uint32_t get_fractional_part(float);                                         //отделяет цифры после запятой
uint16_t low_16bit(uint32_t);                                             //выделяет младшие 16 бит из 32 битной переменной
uint16_t high_16bit(uint32_t);                                            //выделяет старшие 16 бит из 32 битной переменной 
void print_freq(uint64_t, int ,int);
int get_number_of_characters(uint64_t);                                              //возвращает количество цифр в числе 
uint32_t calculation_of_pll_n(uint64_t);
uint32_t calculation_of_pll_num(uint64_t);     
void my_print(uint32_t, uint16_t, uint16_t);