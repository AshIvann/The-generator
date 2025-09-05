#include <Arduino.h>
#include "stdint.h"
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789

//Пины для дислпея 
#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select


extern Adafruit_ST7789 tft;
//   = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
//  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

extern unsigned int divider_values[18];

extern uint64_t number;            //выбор частоты для second_freq
extern uint32_t first_five;
extern uint32_t last_six;


int find_chdiv(uint64_t fout);                                            //определение значения делителя
uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits);
uint8_t fractional(float number);                                         //отделяет цифры после запятой
uint16_t low_16bit(uint32_t);                                             //выделяет младшие 16 бит из 32 битной переменной
uint16_t high_16bit(uint32_t);                                            //выделяет старшие 16 бит из 32 битной переменной 
void print_freq(uint64_t number, int x ,int y);
int getDigitCount(uint64_t);                                              //возвращает количество цифр в числе 
