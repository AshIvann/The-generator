#include "Display.h"

void Display::set_display()
{
  tft.init(240, 320);
  tft.fillScreen(ST77XX_BLACK);   
  tft.setTextColor(ST77XX_WHITE);
  tft.setRotation(1);
  tft.setTextSize(3); 
  tft.fillRect(165, 31, 20, 3, ST77XX_BLUE);
}

void Display::print_freq(uint64_t fr)
{
  uint32_t first_five = fr / (uint32_t)1e6;
  uint32_t last_six = fr % (uint32_t)1e6;
  
  tft.fillRect(10, freq_print_height, SCREEN_WIDTH, 21, ST77XX_BLACK);
  tft.setCursor(10, freq_print_height);
  tft.print("Freq: ");
  tft.print(first_five);
  tft.print(".");
  uint8_t number_of_characters = get_number_of_characters(last_six);
  for(int i = 0; i < 6 - number_of_characters; i++) 
  {
    tft.print(0);
  }
  tft.print(last_six);
  tft.print("MHz");
}

void Display::power_print(uint8_t for_print)
{ 
  tft.fillRect(10, power_print_height, SCREEN_WIDTH, 21, ST77XX_BLACK); 
  tft.setCursor(10,power_print_height);
  tft.print("power: ");
  tft.setCursor(110, power_print_height);
  tft.print(for_print);
  tft.print(" Units");
}

uint8_t Display::get_number_of_characters(uint64_t number)     
{
  if (!number) 
    return 1;
  return (int)(floor(log10(number)) + 1);
}

void Display:: my_print(uint64_t print_number, uint16_t x, uint16_t y)
{
  uint32_t first_five_digits;
  uint32_t last_six_digits;
  first_five_digits = print_number / (uint32_t)1e6;
  last_six_digits = print_number % (uint32_t)1e6;
  
  tft.fillRect(x, y, SCREEN_WIDTH, 21, ST77XX_BLACK);
  tft.print(first_five_digits);
  tft.print(last_six_digits);
}

