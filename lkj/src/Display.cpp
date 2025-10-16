#include "Display.h"

void Display::print_freq(uint64_t fr, uint8_t x ,uint8_t y)
{
  // uint32_t first_five = fr / (uint32_t)1e6;
  uint32_t first_five = fr / (uint32_t)1e6;
  uint32_t last_six = fr % (uint32_t)1e6;
  
  tft.setCursor(x,y);
  tft.fillRect(x, y, SCREEN_WIDTH, 21, ST77XX_BLACK);
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

void Display::power_print(float for_print)
{
  tft.setCursor(110, 85);
  tft.fillRect(110, 85, SCREEN_WIDTH, 21, ST77XX_BLACK); 
  tft.print(for_print);
  tft.print(" Units");
}

uint8_t Display::get_number_of_characters(uint64_t number)     
{
  if (!number) 
    return 1;
  return (int)(floor(log10(number)) + 1);
}










float Display::find_power_level(uint8_t target_power, uint64_t target_freq )
{
  freqs result = detect_index_of_side_freq(target_freq);
  best_pow_level = get_best_level(target_power, target_freq);
  
  if(check_freq[result.right_freq_index] == check_freq[result.left_freq_index])
  {
    power_print(pgm_read_float(&power_table[best_pow_level][result.left_freq_index]));
    return pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
  }
  else
  {
    float out_min = pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
    float out_max = pgm_read_float(&power_table[best_pow_level][result.right_freq_index]);

    float after_my_map = my_map(target_freq, check_freq[result.left_freq_index], check_freq[result.right_freq_index], out_min, out_max);
    power_print(after_my_map);
    return after_my_map;
  }
}

uint8_t Display:: closed_freq(uint64_t target_freq)
{
  uint8_t closest_index = 0;
  //min_diff = abs(1. * (target_freq - check_freq[0]));   //для stm 
  min_diff = abs((target_freq - check_freq[0]));          //для arduino

  for(uint8_t i = 1; i < CHECK_FREQ; i++)                                                              
  {
    uint64_t current_freq = check_freq[i];
    uint64_t diff = abs((target_freq - current_freq)) ;             //для arduino
    // uint64_t diff = abs(1. * (target_freq - current_freq)) ;     //для stm    
  
    if(diff < min_diff)                 
    {
      min_diff = diff;
      closest_index = i;
    }
  }
  return closest_index;
}

uint8_t Display::detect_best_left_level(uint8_t target_power, uint64_t target_freq)
{
  uint8_t best_left_level;
  freqs result = detect_index_of_side_freq(target_freq);
  float best_left_power_diff = abs(target_power - pgm_read_float(&power_table[0][result.left_freq_index]));
  for(uint8_t level = 1; level <= POWER_LEVEL; level++ )
  {
    float current_dbm = pgm_read_float(&power_table[level][result.left_freq_index]);                             
    float diff = abs((target_power - current_dbm));
    if(abs(diff) <= abs(best_left_power_diff))
    {
      best_left_power_diff = diff;
      best_left_level = level;
    }
  }
  return best_left_level;
}

uint8_t Display::detect_best_right_level(uint8_t target_power, uint64_t target_freq)
{
  uint32_t best_right_level;
  freqs result = detect_index_of_side_freq(target_freq);
  float best_right_power_diff = abs(target_power - pgm_read_float(&power_table[0][result.right_freq_index]));
  for(uint8_t level = 1; level <= POWER_LEVEL; level++ )
  {
    float current_dbm = pgm_read_float(&power_table[level][result.right_freq_index]);
    float diff = abs(target_power - current_dbm);
    if(abs(diff) <= abs(best_right_power_diff))
    {
      best_right_power_diff = diff;
      best_right_level = level;
    }
  }
  return best_right_level;
}

uint8_t Display::get_best_level(uint8_t target_power, uint64_t target_freq)
{
  uint8_t best_level;
  uint8_t best_right_level = detect_best_right_level(target_power, target_freq);
  uint8_t best_left_level = detect_best_left_level(target_power, target_freq);
  float best_right_power_diff = rt_power_diff(target_power, target_freq);
  float best_left_power_diff = lt_power_diff(target_power, target_freq);

  if(best_right_power_diff <= best_left_power_diff)
  {
    best_level = best_right_level;
  }
  else
  { 
    best_level = best_left_level;
  }
  return best_level;
}

float Display::rt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_right_level = detect_best_right_level(target_power, result.right_freq_index);

  float right_power_diff = abs(target_power - pgm_read_float(&power_table[best_right_level][result.right_freq_index]));
  
  return right_power_diff;
}

float Display::lt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_left_level = detect_best_left_level(target_power, result.left_freq_index);

  float left_power_diff = abs(target_power - pgm_read_float(&power_table[best_left_level][result.left_freq_index]));
  
  return left_power_diff;
}

Display:: freqs Display::detect_index_of_side_freq(uint64_t target_freq )
{
  freqs side_freq_index;
  if(target_freq <= check_freq[0])
  {
    side_freq_index.left_freq_index = 0;
    side_freq_index.right_freq_index = 0;
    return side_freq_index;
  }
  
  side_freq_index.right_freq_index = side_freq_index.left_freq_index = closed_freq(target_freq);

  if(min_diff == 0)                                     //проверить частоту попадающую ровно в частоты таблицы
    return side_freq_index;                               
  if(target_freq < check_freq[side_freq_index.left_freq_index])
    side_freq_index.left_freq_index = side_freq_index.left_freq_index - 1;
  else
    side_freq_index.right_freq_index = side_freq_index.left_freq_index + 1;
  
  return side_freq_index;
}

float Display:: my_map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// template <typename T> 
// T Display<T>:: my_map(T x, T in_min, T in_max, T out_min, T out_max)
// {
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }