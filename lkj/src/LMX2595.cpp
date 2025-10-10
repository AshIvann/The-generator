#include "LMX2595.h"


uint16_t divider_values[18] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 72, 96, 128, 192, 256, 384, 512, 768};
uint16_t reg_divider[18]  = {0b0000100000000000, 0b0000100001000000, 0b0000100010000000, 0b0000100011000000, 0b0000100100000000, 0b0000100101000000, 0b0000100110000000, 0b0000100111000000, 0b0000101000000000, 0b0000101001000000, 0b0000101010000000, 0b0000101011000000, 0b0000101100000000, 0b0000101101000000, 0b0000101110000000, 0b0000101111000000, 0b0000110000000000, 0b0000110001000000};

void LMX2595::set_freq(uint64_t fout)
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

void LMX2595::writeRegister(uint8_t addr, uint16_t data)
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

byte LMX2595::send_SPI_byte(uint8_t val1)
 {
   uint8_t data_byte;
   data_byte = spi_transfer(val1); //get data byte
   return data_byte;
 }

 char LMX2595::spi_transfer(volatile uint8_t data)
{
  SPDR = data;                    // Start the transmission

  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {};

  return SPDR;                    // return the received byte
}

uint32_t LMX2595::calculation_of_pll_n(uint64_t fout)
{
  st_freq_params st = get_divider_value(fout);
  
  uint16_t n_devider = (st.first_five_of_freq * st.chdiv) / 10 + st.int_part_of_frac_div;   

  return n_devider;
}

uint32_t LMX2595::calculation_of_pll_num(uint64_t fout)      
{
  st_freq_params st = get_divider_value(fout);                                                                                                              
  
  uint32_t full_frac_div = (st.last_six_of_freq * st.chdiv);
  uint32_t frac_part_of_n_div = (((float)st.first_five_of_freq * (float)st.chdiv) / 10.0) - ((st.first_five_of_freq * st.chdiv) / 10);
  uint64_t frac_div = full_frac_div - (st.int_part_of_frac_div * 10e6) + (frac_part_of_n_div * 10e6); 

  return frac_div;
}

uint16_t LMX2595::low_16bit(uint32_t value) 
{
  // Младшая 16-битная часть
  uint16_t low  = (uint16_t)(value & 0xFFFF);
  return low; 
}

uint16_t LMX2595::high_16bit(uint32_t value) 
{
  // Старшая 16-битная часть
  uint16_t high = (uint16_t)((value >> 16) & 0xFFFF);    
  return high;
}

uint8_t LMX2595::find_chdiv(uint64_t fout)
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

LMX2595::st_freq_params LMX2595:: get_divider_value(uint64_t fout)
{
  st_freq_params st;
  
  st.first_five_of_freq = fout / 1000000;
  st.last_six_of_freq = fout % 1000000;
  st.chdiv = divider_values[find_chdiv(fout)];
  st.int_part_of_frac_div = (st.last_six_of_freq * st.chdiv) / 10e6;
  return st;
}



void LMX2595::set_out_power(uint16_t power)
{
  writeRegister(R44, replace_bits_8_to_13(0x1EA3, power));
}

uint16_t LMX2595::replace_bits_8_to_13(uint16_t original, uint8_t new_bits) 
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

void LMX2595::set_generator(uint64_t fout, uint8_t power)
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
