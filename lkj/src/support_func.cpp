#include "support_func.h"

#define SCREEN_HEIGHT 240
#define SCREEN WIDTH 360 


unsigned int divider_values[18] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 72, 96, 128, 192, 256, 384, 512, 768};


const byte ROWS = 4;   // Количество рядов 
const byte COLS = 4;   // Количество строк 

char keys[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A3, A2, A1, A0}; // Выводы, подключение к строкам
byte colPins[COLS] = {6, 5, A5, A4}; // Выводы, подключение к столбцам  

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Encoder enc1(CLK, DT, SW, TYPE2);
uint64_t freq_set_by_encoder = 25e6;


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
  return 0;     
}

uint32_t get_fractional_part(float number)          //отделяет цифру после запятой
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

int get_number_of_characters(uint64_t number)     
{
  if (!number) 
    return 1;

  return (int)(floor(log10(number)) + 1);
}

void print_freq(uint64_t fr, int x ,int y)
{
  uint32_t first_five = fr / 1e6;
  uint32_t last_six = fr % (uint32_t)1e6;
  
  tft.setCursor(x,y);
  tft.fillRect(x, y, 320, 21, ST77XX_BLACK);
  tft.print(first_five);
  tft.print(".");

  uint8_t number_of_charaacters = get_number_of_characters(last_six);
  for(int i = 0; i < 6 - number_of_charaacters; i++) 
  {
    tft.print(0);
  }

  tft.print(last_six);
  tft.print("MHz");
}

uint32_t calculation_of_pll_n(uint64_t fout)
{
  uint32_t first_five_of_freq = fout / 1000000;
  uint32_t last_six_of_freq = fout % 1000000;
  chdiv = divider_values[find_chdiv(fout)];

  uint32_t integer_part_of_fractional_divider = (last_six_of_freq * chdiv) / PLL_DEN;
  uint16_t n_devider = (first_five_of_freq * chdiv) / FREQ_OF_PHASE_DETECTOR + integer_part_of_fractional_divider;   //10 это частота фазового детектора

  return n_devider;
}

uint32_t calculation_of_pll_num(uint64_t fout)      
{
uint32_t first_five_of_freq = fout / 1000000;
uint32_t last_six_of_freq = fout % 1000000;
chdiv = divider_values[find_chdiv(fout)];                                                                                                                        //не знаю нужно ли это повторно писать, если эта функция идет следом за calculation_of_pll_n, где уже расчитывается эта переменная 
  
uint32_t NUM = (last_six_of_freq * chdiv) / PLL_DEN;
uint32_t fractional_part_of_fractional_divider = (last_six_of_freq * chdiv) - NUM * 1e7;
uint32_t n_devider = (((float)first_five_of_freq * (float)chdiv) / 10.0);

uint64_t fractional_divider = fractional_part_of_fractional_divider + get_fractional_part(n_devider) * 1e6;        //дробная часть от NUM. (int) NUM * 1e7 необходимо, чтобы убрать целую часть(если она есть), так как ее я уже прибавил к pll_n

return fractional_divider;
}