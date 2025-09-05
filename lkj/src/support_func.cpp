#include "support_func.h"


unsigned int divider_values[18] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 72, 96, 128, 192, 256, 384, 512, 768};
uint32_t last_six = number % 1000000;
uint32_t first_five = number / 1000000;
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits) 
{
  // Маска для очистки битов 8-13: 0b1100000111111111
  uint16_t mask = 0xC0FF;
  new_bits &= 0x3F;
  // Сдвигаем новые биты на позицию 8
  uint16_t shifted_bits = (uint16_t)new_bits << 8;
  // Очищаем биты 8-13 и вставляем новые
  uint16_t final = (original & mask) | shifted_bits;

  return final;
}

int find_chdiv(uint64_t fout)
{
  for(int N = 0; N <= 17; N++)
  {
    uint64_t left = 7500 * 1e6 / divider_values[N];
    if(fout >= left) 
    {
      return N;
    }
  }
  //return 0;     
}

uint8_t fractional(float number)          //отделяет цифру после запятой
{
  
    int integer_part = (int)floor(number); // Целая часть
    float fractional_part = number - integer_part + 0.01; // Дробная часть
    int fractional_digits = (int)(fractional_part * 10); // Умножаем на 100, чтобы получить 1
    return fractional_digits;
}

uint16_t low_16bit(uint32_t value) 
{
    // Младшая 16-битная часть
    uint16_t low  = (uint16_t)(value & 0xFFFF);
    return low; 
}

uint16_t high_16bit(uint32_t value) 
{
    // Старшая часть (в примере для 32-битного числа хватит одного сдвига)
    uint16_t high = (uint16_t)((value >> 16) & 0xFFFF);    
    return high;
}

int getDigitCount(uint64_t number)     
{
  if (number == 0) 
  {
    return 1;
  }
  return (int)(floor(log10(number)) + 1);
}

void print_freq(uint64_t number, int x ,int y)
{
  first_five = number / 1e6;
  last_six = number % (uint32_t)1e6;
  tft.setCursor(x,y);
  tft.fillRect(x, y, 320, 21, ST77XX_BLACK);
  tft.print(first_five);
  tft.print(".");
  for(int i = 0; i < 6 - getDigitCount(last_six); i++)
  {
    tft.print(0) ;
  }
  tft.print(last_six);
  tft.print("MHz");
}