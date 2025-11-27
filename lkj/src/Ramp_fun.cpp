#include "LMX2595.h"

void LMX2595:: dif_ramp_R101_R97_RAMP_RST_ON(uint32_t step, uint16_t len)                                               //изменил R101 double_lenght
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b00000000001110000);                                                                                                 // Double ramp length - off
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    
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
void LMX2595:: dif_ramp_R101_R97_RAMP_RST_OFF(uint32_t step, uint16_t len)                                               //изменил R101 double_lenght
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b00000000001010000);                                                                                                 // Double ramp length - off
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b0000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b0000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    
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


void LMX2595:: dif_ramp_R38_R39_PLL_DEN(uint32_t step, uint16_t len)                                               //изменил R101 double_lenght
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
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0] 
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);                        //RAMP_LIMIT_HIGH[0]    
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000001000000001);       //RAMP_THRESH[11]
  writeRegister(R75, 0b0000100111000000);       //CHDIV = 32
  writeRegister(R45, 0b1100011011011111);       //OUTA_MUX = Channel divider, OUT_ISET = max, OUTB_PWR = 31
  writeRegister(R43, 0);                        //PLL_NUM = 0
  writeRegister(R42, 0);                        //PLL_NUM = 0
  writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}

void LMX2595:: dif_ramp_Ramp_trash(uint32_t step, uint16_t len)
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
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0] 
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    

  writeRegister(R80, 0x0000);                   //RAMP_THRESH[15:0] = 4 194 304
  writeRegister(R79, 0x0040);                   //RAMP_THRESH[31:16] = 4 194 304
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


void LMX2595:: dif_ramp_ramp_lim(uint32_t step, uint16_t len)
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
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0xFFFF);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0xFFFF);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0x0001);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0

  writeRegister(R83, 0xFFFF);                   //RAMP_LIMIT_HIGH[15:0] 
  writeRegister(R82, 0xFFFF);                   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    
   
  writeRegister(R80, low_16bit(16777216));      //RAMP_THRESH[15:0]
  writeRegister(R79, high_16bit(16777216));     //RAMP_THRESH[31:16]
  writeRegister(R78, 0b0000101000000001);       //RAMP_THRESH[11]
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

void LMX2595:: dif_ramp_R106(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0x0012);                       //RAMP_TRIG_CAL = 1
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
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]  
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    
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

void LMX2595:: dif_ramp_R105(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0xFA00);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0] 
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    
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

void LMX2595:: dif_ramp_R101_double_ramp_lenght(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000011100000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
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


void LMX2595:: dif_ramp_R101_single_ramp_lenght(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0);                       //RAMP_TRIG_CAL = 0
  writeRegister(R105, 0);                       //RAMP_MANUAL = 0, RAMP1_NEXT = 0, RAMP1_NEXT_TRIG
  writeRegister(R104, len);                                 //RAMP1_LEN
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000001100000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


  writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0,  RAMP0_RST = 1
  writeRegister(R96, 0b1000001011111100);       //включил BURST(который определяет количество ramps), количество = 191    
  // writeRegister(R97, 0b1000100000000000);        //RAMP_BURST_TRIG = 0(стр 36),  RAMP0_RST = 1(стр 36) 
  // writeRegister(R96, 0);                        //RAMP_BURST_COUNT = 0(стр 36), RAMP_BURST_EN = 0


  writeRegister(R86, 0);                        //RAMP_LIMIT_LOW[15:0] 
  writeRegister(R85, 0);                        //RAMP_LIMIT_LOW[31:16]
  writeRegister(R84, 0);                        //RAMP_LIMIT_LOW[0]      на стр 32 написано, что может быть 0
  writeRegister(R83, low_16bit(2516582400));    //RAMP_LIMIT_HIGH[15:0]   значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R82, high_16bit(2516582400));   //RAMP_LIMIT_HIGH[31:16]  значение нашел по формуле на стр 57 при Fhigh = 9 GHz, а Fvco = 7.5 GHz
  writeRegister(R81, 0);       //RAMP_LIMIT_HIGH[0]    возможно из-за этой 1 получу не 2516582400, а 6811549696 
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


void LMX2595:: ramp(uint32_t step, uint16_t len)
{
  writeRegister(R106, 0);                       
  writeRegister(R105, 0);                       
  writeRegister(R104, len);                                 
  writeRegister(R103, low_16bit(calcul_ramp1_inc(step, len)));   //RAMP1_INC
  writeRegister(R102, high_16bit(calcul_ramp1_inc(step, len)));  //максимум = 1 073 741 823
  writeRegister(R101, 0b0000000001110000);     // Double ramp length
  writeRegister(R100, len);                   //RAMP0_LEN
  writeRegister(R99, low_16bit(calcul_ramp0_inc(step, len)));            //RAMP0_INC = 335540
  writeRegister(R98, write_98reg(calcul_ramp0_inc(step, len)));          //максимум = 1 073 741 823


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
  // writeRegister(R39, 0x0000);                   //PLL_DEN = 16777216
  // writeRegister(R38, 0x100);                    //PLL_DEN = 16777216
  writeRegister(R39, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R38, 0xffff);                   //PLL_DEN = 4.294.967.295
  writeRegister(R36, 375);                      //PLL_N, чтобы начальная частота VCO = 7500 MHz 
  writeRegister(R31, 0x43EC);                   //SEG1_EN = 1
  writeRegister(R12, 0x5001);                   //PLL_R_PRE = 1
  writeRegister(R9, 0x1604);                    //OSC_2X = 1
  writeRegister(R0, 0b1010011000011100);        //enable ramp mode
  
}


