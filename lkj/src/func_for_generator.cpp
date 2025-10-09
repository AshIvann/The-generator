#include "func_for_generator.h"
#include "support_func.h"
#include "power_table.h"
#include "ard_nano.h"

uint64_t frq_set_by_encoder = 25e6;

unsigned int reg_divider[18]  = {0b0000100000000000, 0b0000100001000000, 0b0000100010000000, 0b0000100011000000, 0b0000100100000000, 0b0000100101000000, 0b0000100110000000, 0b0000100111000000, 0b0000101000000000, 0b0000101001000000, 0b0000101010000000, 0b0000101011000000, 0b0000101100000000, 0b0000101101000000, 0b0000101110000000, 0b0000101111000000, 0b0000110000000000, 0b0000110001000000};
int chdiv_reg = 0;
uint64_t min_diff = 0;

void set_out_power(uint16_t power)
{
  writeRegister(R44, replace_bits_8_to_13(0x1EA3, power));
}

void writeRegister(uint8_t addr, uint16_t data)
{
  struct st_packet 
  {
    uint8_t addr : 7;
    uint8_t rw : 1;             
    uint16_t data;
  };
  st_packet packet;

  packet.rw = 0;           // Режим записи при 0, чтение при 1                  
  packet.addr = addr;       // Адрес регистра
  packet.data = data;       // данные 

  uint8_t* bytes = (uint8_t*)&packet;

  digitalWrite(CHIPSELECT,LOW);
  send_SPI_byte(bytes[0]);
  send_SPI_byte(bytes[2]);
  send_SPI_byte(bytes[1]);
  digitalWrite(CHIPSELECT,HIGH);  //release chip, signal end transfer
}

byte send_SPI_byte(uint8_t val1)
 {
   uint8_t data_byte;
   data_byte = spi_transfer(val1); //get data byte
   return data_byte;
 }

void set_generator(uint64_t fout, uint8_t power)
{
  uint64_t pll_n = calculation_of_pll_n(fout);
  uint64_t fractional_divider = calculation_of_pll_num(fout);                   
  chdiv_reg = reg_divider[find_chdiv(fout)];

  // Program RESET = 1 to reset registers
  writeRegister(R0, 0b0010010000011110);
  //Program RESET = 0 to remove reset
  writeRegister(R0, 0b0010010000011100);

  writeRegister(R78, 0x0003);
  writeRegister(R75, chdiv_reg);   
  writeRegister(R74, 0x0000);
  writeRegister(R73, 0x003F);
  writeRegister(R72, 0x0001);
  writeRegister(R71, 0x0081);
  writeRegister(R70, 0xC350);
  writeRegister(R69, 0x0000);
  writeRegister(R60, 0x0000);
  writeRegister(R59, 0x0001);
  writeRegister(R46, 0x07FC);
  //writeRegister(R45, 0b1101000011011110);   //No output power boost
  writeRegister(R45, 0b1100000011011110);  //Maximum output power boost
  //writeRegister(R44, 0b0001111010100011);   //OUTA_PWR =30
  set_out_power(power);
  writeRegister(R43, low_16bit(fractional_divider));
  writeRegister(R42, high_16bit(fractional_divider));
  writeRegister(R41, 0x0000);
  writeRegister(R40, 0x0000);
  writeRegister(R39, 0x9680);
  writeRegister(R38, 0x0098);
  writeRegister(R37, 0x0304);
  writeRegister(R36, pll_n);
  writeRegister(R34, 0x0000);
  writeRegister(R31, 0x43EC);
  writeRegister(R27, 0x0002);
  writeRegister(R20, 0xE048);
  writeRegister(R19, 0x27B7);
  writeRegister(R17, 0x012C);
  writeRegister(R16, 0x0080);        
  writeRegister(R14, 0x1E70);
  writeRegister(R12, 0x5002);
  writeRegister(R11, 0x0018);
  writeRegister(R10, 0x10D8);
  writeRegister(R9,  0x1604);
  writeRegister(R8,  0x2000);
  writeRegister(R7,  0x40B2);
  writeRegister(R1,  0x0808);
  writeRegister(R0,  0b0010010000011100);            
}

void set_freq(uint64_t fout)
{
  if(fout < 19e9 && fout > 15e9)   //VCO doubler
  {
    //VCO doubler 
    writeRegister(R45, 0b1101000011011110);   //переключил выход A на VCO Doubler
    writeRegister(R27, 0b0000000000000011);   //включил VCO2X_EN
    return;
  }
  
  if(fout <15e9 && fout >75e9 )    //VCO
  {
    //VCO
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //выключил VCO2X_EN    
    return;
  }

  // else        //<7500
    writeRegister(R46, 0b0000011111111100);   //переключил выход B на Channel Divider
    writeRegister(R45, 0b1100000011011110);   //переключил выход A на Channel Divider
    writeRegister(R31, 0b0100001111101100);   //включил CHDIV

    chdiv_reg = reg_divider[find_chdiv(fout)];

    uint64_t pll_n = calculation_of_pll_n(fout);
    uint64_t fractional_divider = calculation_of_pll_num(fout);
    
    writeRegister(R75, chdiv_reg);                                                            //проверить все ли нормально для частот >7500
    writeRegister(R36, pll_n);
    writeRegister(R43, low_16bit(fractional_divider));
    writeRegister(R42, high_16bit(fractional_divider));

    writeRegister(R0, 0b0010010000011100);
    return;
}

freqs detect_index_of_side_freq(uint64_t target_freq )
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

float find_power_level(uint8_t target_power, uint64_t target_freq )
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint8_t best_pow_level = get_best_level(target_power, target_freq);
  
  /*
  Serial.print("best_pow_level = ");
  Serial.println(best_pow_level);
  
  Serial.print("out_min = ");
  Serial.println(pgm_read_float(&power_table[best_pow_level][result.left_freq_index]));
  
  Serial.print("out_max = ");
  Serial.println(pgm_read_float(&power_table[best_pow_level][result.right_freq_index]));

  Serial.print("lt_freq ");
  Serial.println((uint32_t)check_freq[result.left_freq_index] / 10000);
  
  Serial.print("rt_freq ");
  Serial.println((uint32_t)check_freq[result.right_freq_index] / 10000);
*/

  if(check_freq[result.right_freq_index] == check_freq[result.left_freq_index])
  {
    Serial.print("best_pow = ");
    Serial.println(pgm_read_float(&power_table[best_pow_level][result.left_freq_index]), 4);
    return pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
  }
  else
  {
    float out_min = pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
    float out_max = pgm_read_float(&power_table[best_pow_level][result.right_freq_index]);

    float after_my_map = my_map<float>(target_freq, check_freq[result.left_freq_index], check_freq[result.right_freq_index], out_min, out_max);
    Serial.print("after_my_map = ");
    Serial.println(after_my_map, 4);
    return after_my_map;
  }
}

template <typename T> T my_map(T x, T in_min, T in_max, T out_min, T out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t closed_freq(uint64_t target_freq)
{
  uint8_t closest_index = 0;
  //min_diff = abs(1. * (target_freq - check_freq[0]));   //для stm 
  min_diff = abs((target_freq - check_freq[0]));          //для arduino

  for(uint8_t i = 1; i < 42; i++)                                                              
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

uint8_t detect_best_left_level(uint8_t target_power, uint64_t target_freq)
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

uint8_t detect_best_right_level(uint8_t target_power, uint64_t target_freq)
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

uint8_t get_best_level(uint8_t target_power, uint64_t target_freq)
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

float rt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_right_level = detect_best_right_level(target_power, result.right_freq_index);

  float right_power_diff = abs(target_power - pgm_read_float(&power_table[best_right_level][result.right_freq_index]));
  
  return right_power_diff;
}

float lt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_left_level = detect_best_left_level(target_power, result.left_freq_index);

  float left_power_diff = abs(target_power - pgm_read_float(&power_table[best_left_level][result.left_freq_index]));
  
  return left_power_diff;
}