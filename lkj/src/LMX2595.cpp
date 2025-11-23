#include "LMX2595.h"


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
  uint32_t jjj = pll_n;
  uint64_t fractional_divider = calculation_of_pll_num(fout);
  uint32_t bbb = fractional_divider;
   Serial.print("pll_n = ");
  Serial.println(jjj);
    Serial.print("fractional_divider = ");
  Serial.println(bbb);
  writeRegister(R75, chdiv_reg);                                                            //проверить все ли нормально для частот >7500
  writeRegister(R36, pll_n);
  writeRegister(R43, low_16bit(fractional_divider));
  writeRegister(R42, high_16bit(fractional_divider));
  writeRegister(R0, 0b0010010000011100);
  return;
}

void LMX2595::power_increas(void)
{
  power_value++;
}

void LMX2595::power_decreas(void)
{
  power_value--;
}

void LMX2595:: freq_increas(void)
{
  freq_value++;
}

void LMX2595::freq_decreas(void)
{
  freq_value--;
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
  
  uint16_t n_devider = (st.first_five_of_freq * st.chdiv) / phase_detector_freq + st.int_part_of_frac_div; 
  Serial.print("st.chdiv = ");
  Serial.println(st.chdiv);
  // Serial.print("first_five_of_freq = ");
  // Serial.println(st.first_five_of_freq );  
   
  return n_devider;
}

uint32_t LMX2595::calculation_of_pll_num(uint64_t fout)      
{
    st_freq_params st = get_divider_value(fout);                                                                                                              
  
  uint32_t full_frac_div = (st.last_six_of_freq * st.chdiv) / (1000000.0 * phase_detector_freq) * 1e6;
  double float_frac_part_of_n_div = (((float)st.first_five_of_freq * (float)st.chdiv) / phase_detector_freq);

  uint32_t frac_part_of_n_div = ((float_frac_part_of_n_div + 0.01) - ((st.first_five_of_freq * st.chdiv) / phase_detector_freq)) * 10;
  uint32_t frac_div = (full_frac_div - (st.int_part_of_frac_div * 1000000) + (frac_part_of_n_div * 100000));// * 10;            //нужно проверить нужна ли эта 10 и в каких случаях

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
  st.int_part_of_frac_div = (st.last_six_of_freq * st.chdiv) / (1000000 * phase_detector_freq);
  // Serial.print("st.chdiv = ");
  // Serial.println(st.chdiv);
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


void LMX2595:: reset()
{
  writeRegister(R0, 0b0010010000011110);        //reset = 1
}

void LMX2595:: set_ramp1() //вроде как тут только ramp1 и шаг 1073406284 = 335540
{
  //  writeRegister(R0, 0b0010010000011110);        //RESET = 1
  //  writeRegister(R0, 0b0010010000011100);        //RESET = 0
  /*
  writeRegister(R9, 0x0604);                    //OSC_2X = 1
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1

  writeRegister(R36, 750);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0

  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG

  writeRegister(R104, 50000);                   //RAMP1_LEN

  writeRegister(R102, high_16bit(1073728402));  //RAMP1_INC = 1073728402 (стр 36)
  writeRegister(R103, low_16bit(1073728402));   //RAMP1_INC = 1073728402 (стр 36)
  writeRegister(R101, 0b0000000000010000);

  writeRegister(R100, 50000);                   //RAMP0_LEN(стр 36)

  writeRegister(R98, 0);                        //RAMP0_DLY = 0(стр 36)
  writeRegister(R99, 13422);                    //RAMP0_INC

  writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0

  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  
  writeRegister(R81, 0b0000000000000001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16] значение нашел по формуле на стр 57 при Fhigh = 9 GHz 
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   а Fvco = 7.5 GHz
  
  writeRegister(R78, 0b0000101000000001);       //RAMP_THRESH[11]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]

*/


  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0b0000000000010000);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, 0xffff);                   //RAMP1_LEN
  writeRegister(R103, low_16bit(1073406284));   //RAMP1_INC = 1073741790 (стр 36)
  writeRegister(R102, high_16bit(1073406284));  //RAMP1_INC = 1073741790 (стр 36)
  writeRegister(R101, 0b0000000000010000);      //RAMP0_NEXT_TRIG = 0, RAMP0_NEXT = 1, RAMP1_RST =0(стр36)
  writeRegister(R100, 0xFFFF);                   //RAMP0_LEN(стр 36)
  writeRegister(R99, 0x1EB4);                    //RAMP0_INC = 335540
  writeRegister(R98, 0xff15);                        //RAMP0_DLY = 1(стр 36)
  writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0
  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0b0000000000000001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000101000000001);       //RAMP_THRESH[11]

  writeRegister(R45, 0b1100011011000000);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 0
  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32

  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0xffff);                   //PLL_DEN = 
  writeRegister(R38, 0xffff);                    //PLL_DEN = 
  writeRegister(R36, 750);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x0604);                    //OSC_2X = 2
  writeRegister(R0, 0b1010010000011100);        //enable ramp mode
}

void LMX2595:: set_ramp2() //RAMP1_RST = 1; 
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0b0000000000000000);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, 0xffff);                   //RAMP1_LEN
  writeRegister(R103, low_16bit(1073406284));   //RAMP1_INC = 1073741790 (стр 36)
  writeRegister(R102, high_16bit(1073406284));  //RAMP1_INC = 1073741790 (стр 36)
  writeRegister(R101, 0b0000000000110000);      //RAMP0_NEXT_TRIG = 0, RAMP0_NEXT = 1, RAMP1_RST =0(стр36)
  writeRegister(R100, 0xffff);                   //RAMP0_LEN(стр 36)
  writeRegister(R99, 34);                    //RAMP0_INC
  writeRegister(R98, 0b0000000000000001);                        //RAMP0_DLY = 1(стр 36)
  writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0
  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0b0000000000000001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000101000000001);       //RAMP_THRESH[11]

  writeRegister(R45, 0b1100011011000000);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 0
  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32

  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 
  writeRegister(R38, 0x100);                    //PLL_DEN = 
  writeRegister(R36, 750);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x0604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
}


uint64_t LMX2595:: calcul_ramp0_inc(uint64_t ramp_step, uint16_t ramp_len)
{
  uint64_t ramp0_inc = (ramp_step * pll_den)/(phase_detector_freq * 1000000 * ramp_len);
  return ramp0_inc;
}

uint64_t LMX2595:: calcul_ramp1_inc(uint64_t ramp_step, uint16_t ramp_len)
{
  uint64_t ramp1_inc = 1073741824 - calcul_ramp0_inc(ramp_step, ramp_len);
  return ramp1_inc;
}


void LMX2595::set_ramp3()                                                                                                               //Fpd = 20, Fvco = 7500, N_DEV = 375,  
{
  
  writeRegister(R106, 0b0000000000000000);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0b0000000000000000);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, 0x07D0);                                                                                                          //RAMP1_LEN
  writeRegister(R103, low_16bit(1073699881));                                                                                           //RAMP1_INC = 1073699881 (стр 36)
  writeRegister(R102, high_16bit(1073699881));                                                                                          //RAMP1_INC = 1073699881 (стр 36)
  writeRegister(R101, 0b0000000000010000);      //RAMP0_NEXT_TRIG = 0, RAMP0_NEXT = 1, RAMP1_RST =0(стр36)
  writeRegister(R100, 0x07D0);                                                                                                          //RAMP0_LEN
  
  writeRegister(R99, 0xA3D7);                                                                                                           //RAMP0_INC = 41943
  writeRegister(R98, 0b0000000000000001);                                                                                               //RAMP0_DLY = 1(стр 36)
  
  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0b0000000000000001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000101000000001);       //RAMP_THRESH[11]

  writeRegister(R45, 0b1100011011000000);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 0
  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R11, 0x0018);
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                                                                                                                                                        //OSC_2X = 2
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}

void LMX2595::set_ramp4()                                                                                                               //Fpd = 20, Fvco = 7500, N_DEV = 375,  
{
  
    writeRegister(R106, 0b0000000000000000);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0b0000000000000000);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, 0x07D0);                                                                                                          //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(100000000, 2000)));                                                                                           //RAMP1_INC = 1073699881 (стр 36)
  writeRegister(R102, high_16bit(calcul_ramp1_inc(100000000, 2000)));                                                                                          //RAMP1_INC = 1073699881 (стр 36)
  writeRegister(R101, 0b0000000000110000);      //RAMP0_NEXT_TRIG = 0, RAMP0_NEXT = 1, RAMP1_RST =0(стр36)
  writeRegister(R100, 0x07D0);                                                                                                          //RAMP0_LEN
  
  writeRegister(R99, low_16bit(calcul_ramp0_inc(100000000, 2000)));                                                                                                           //RAMP0_INC = 41943
  writeRegister(R98, write_98reg(calcul_ramp0_inc(100000000, 2000)));                                                                                               //RAMP0_DLY = 1(стр 36)
  
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  // writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0x0001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R80, 0x0000);      //RAMP_THRESH[15:0]
  writeRegister(R79, 0x0180);     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]

  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 31
  // writeRegister(R45, 0b1100011011000000);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 0
  
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  writeRegister(R36, 384);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R11, 0x0018);
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                                                                                                                                                        //OSC_2X = 2
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}

void LMX2595:: dif_ramp(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b100010000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0b0000000000000001);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]
  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 31
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}

uint32_t LMX2595:: write_98reg(uint64_t incr)
{
  uint16_t high_bit = high_16bit(incr);
  uint16_t result = high_bit & 0b1111111111111101  ;
return result;
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
  writeRegister(R45, 0b1100000011011110);  //Maximum output power boost
  set_out_power(power);
  writeRegister(R43, low_16bit(fractional_divider));
  writeRegister(R42, high_16bit(fractional_divider));
  writeRegister(R41, 0x0000);
  writeRegister(R40, 0x0000);
  writeRegister(R39, 0x9680);
  writeRegister(R38, 0x0098);
  writeRegister(R37, 0x0304);
  // writeRegister(R37, 0x0b1000000000000100);  //проверить как на эта строка повлияет на spurs(изменил MASH_SEED_EN)
  writeRegister(R36, pll_n);
  writeRegister(R34, 0x0000);
  writeRegister(R31, 0x43EC);
  writeRegister(R27, 0x0002);
  writeRegister(R20, 0xE048);
  writeRegister(R19, 0x27B7);
  writeRegister(R17, 0x012C);
  writeRegister(R16, 0x0080);        
  //writeRegister(R14, 0x1E70);
  writeRegister(R14, 0x1E10);                         //charge pump = 6mA, проверить как будет выглядеть сигнал, раньше был 15 
  writeRegister(R12, 0x5001);
  writeRegister(R11, 0x0018);
  writeRegister(R10, 0x10D8);
  writeRegister(R9,  0x1604);                         //OSC_2X - Enabled
  writeRegister(R8,  0x2000);
  writeRegister(R7,  0x40B2);
  writeRegister(R1,  0x0808);
  writeRegister(R0,  0b0010010000011100);            
}


float LMX2595::find_power_level(uint8_t target_power, uint64_t target_freq )
{
  freqs result = detect_index_of_side_freq(target_freq);
  best_pow_level = get_best_level(target_power, target_freq);
  
  if(check_freq[result.right_freq_index] == check_freq[result.left_freq_index])
  {
    // power_print(pgm_read_float(&power_table[best_pow_level][result.left_freq_index]));
    return pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
  }
  else
  {
    float out_min = pgm_read_float(&power_table[best_pow_level][result.left_freq_index]);
    float out_max = pgm_read_float(&power_table[best_pow_level][result.right_freq_index]);

    float after_my_map = my_map(target_freq, check_freq[result.left_freq_index], check_freq[result.right_freq_index], out_min, out_max);
    // power_print(after_my_map);
    return after_my_map;
  }
}

uint8_t LMX2595:: closed_freq(uint64_t target_freq)
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

uint8_t LMX2595::detect_best_left_level(uint8_t target_power, uint64_t target_freq)
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

uint8_t LMX2595::detect_best_right_level(uint8_t target_power, uint64_t target_freq)
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

uint8_t LMX2595::get_best_level(uint8_t target_power, uint64_t target_freq)
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

float LMX2595::rt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_right_level = detect_best_right_level(target_power, result.right_freq_index);

  float right_power_diff = abs(target_power - pgm_read_float(&power_table[best_right_level][result.right_freq_index]));
  
  return right_power_diff;
}

float LMX2595::lt_power_diff(uint8_t target_power, uint64_t target_freq)
{
  freqs result = detect_index_of_side_freq(target_freq);
  uint32_t best_left_level = detect_best_left_level(target_power, result.left_freq_index);


  float left_power_diff = abs(target_power - pgm_read_float(&power_table[best_left_level][result.left_freq_index]));
  
  return left_power_diff;
}

LMX2595:: freqs LMX2595::detect_index_of_side_freq(uint64_t target_freq )
{
  freqs side_freq_index;
  if(target_freq <= check_freq[0])
  {
    side_freq_index.left_freq_index = 0;
    side_freq_index.right_freq_index = 0;
    return side_freq_index;
  }
  
  side_freq_index.right_freq_index = side_freq_index.left_freq_index = closed_freq(target_freq);

  if(min_diff == 0)                                                                         //проверить частоту попадающую ровно в частоты таблицы
    return side_freq_index;                               
  if(target_freq < check_freq[side_freq_index.left_freq_index])
    side_freq_index.left_freq_index = side_freq_index.left_freq_index - 1;
  else
    side_freq_index.right_freq_index = side_freq_index.left_freq_index + 1;
  
  return side_freq_index;
}

float LMX2595:: my_map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
