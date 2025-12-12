#include "LMX2595.h"

void LMX2595:: ramp(uint64_t step, uint16_t len)
{
  writeRegister(R106, 0);                       
  writeRegister(R105, 0);                       
  writeRegister(R104, len);                                 
  writeRegister(R103, low_16bit(1072494721));   //RAMP1_INC
  writeRegister(R102, high_16bit(1072494721));  //максимум = 1 073 741 823  
  // writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  // writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000000110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(1247103));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(1247103, 0));          //максимум = 1 073 741 823
  //   writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  // writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len), 1));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0x0001);                        //RAMP_LIMIT_LOW[0]      

  writeRegister(R83, 0xFFFF);    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, 0xFFFF);   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]   

  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]

  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  // writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  // writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R14, 0x1E40);                                                                                                 //уменьшает уровни второстепенных частот 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}
uint32_t gd = 1073574052;
uint32_t sfg = 167772;
void LMX2595:: ramp2(uint64_t step, uint16_t len)
{
  writeRegister(R106, 0);                       
  writeRegister(R105, 0);                       
  writeRegister(R104, len);               
  uint32_t incr1 = calcul_ramp1_inc(step, len)  ;
  writeRegister(R103, low_16bit(incr1));   //RAMP1_INC
  writeRegister(R102, high_16bit(incr1));  //максимум = 1 073 741 823


  // writeRegister(R103, low_16bit(1073574052));   //RAMP1_INC
  // writeRegister(R102, high_16bit(1073574052));  //максимум = 1 073 741 823


  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
   incr1 = calcul_ramp0_inc(400000000, 2000);
  writeRegister(R99, low_16bit(incr1));
  writeRegister(R98, write_98reg(incr1, 1));
  
  // writeRegister(R99, low_16bit(167772));            //RAMP0_INC = 335540
  // writeRegister(R98, write_98reg(167772, 1));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0x0001);                        //RAMP_LIMIT_LOW[0]      

  writeRegister(R83, 0xFFFF);    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, 0xFFFF);   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]   

  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]

  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  // writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  // writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R14, 0x1E40);                                                                                                 //уменьшает уровни второстепенных частот 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}


void LMX2595:: ramp3(uint64_t step, uint16_t len)
{
  writeRegister(R106, 0);                       
  writeRegister(R105, 0);                       
  writeRegister(R104, len);                              
  writeRegister(R103, low_16bit(1073574052));   //RAMP1_INC
  writeRegister(R102, high_16bit(1073574052));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(167772));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(167772, 1));          //максимум = 1 073 741 823


  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  // writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0x0001);                        //RAMP_LIMIT_LOW[0]      

  writeRegister(R83, 0xFFFF);    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, 0xFFFF);   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]   

   writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
   writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
   writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]

  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  // writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  // writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R14, 0x1E40);                                                                                                 //уменьшает уровни второстепенных частот 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}


void LMX2595:: ramp4(uint64_t step, uint16_t len)
{
  writeRegister(R106, 0);                       
  writeRegister(R105, 0);                       
  writeRegister(R104, len);                                 
  writeRegister(R103, low_16bit(1073574052));   //RAMP1_INC
  writeRegister(R102, high_16bit(1073574052));  //максимум = 1 073 741 823


  // writeRegister(R103, low_16bit(1073574052));   //RAMP1_INC
  // writeRegister(R102, high_16bit(1073574052));  //максимум = 1 073 741 823


  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  
  writeRegister(R99, low_16bit(167772));
  writeRegister(R98, write_98reg(167772, 1));
  
  // writeRegister(R99, low_16bit(167772));            //RAMP0_INC = 335540
  // writeRegister(R98, write_98reg(167772, 1));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0x0001);                        //RAMP_LIMIT_LOW[0]      

  writeRegister(R83, 0xFFFF);    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, 0xFFFF);   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]   

  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]

  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  // writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  // writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R14, 0x1E40);                                                                                                 //уменьшает уровни второстепенных частот 
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}
