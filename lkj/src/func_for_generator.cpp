#include "func_for_generator.h"
#include "support_func.h"


uint64_t number = 25e6;

unsigned int reg_divider[18]  = {0b0000100000000000, 0b0000100001000000, 0b0000100010000000, 0b0000100011000000, 0b0000100100000000, 0b0000100101000000, 0b0000100110000000, 0b0000100111000000, 0b0000101000000000, 0b0000101001000000, 0b0000101010000000, 0b0000101011000000, 0b0000101100000000, 0b0000101101000000, 0b0000101110000000, 0b0000101111000000, 0b0000110000000000, 0b0000110001000000};
int chdiv = 0;
int chdiv_reg = 0;


void set_power(uint16_t power)
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

  //digitalWrite(CS, LOW);

  digitalWrite(CHIPSELECT,LOW);
  send_SPI_byte(bytes[0]);
  send_SPI_byte(bytes[2]);
  send_SPI_byte(bytes[1]);
  digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer

 // digitalWrite(CS, HIGH);
}

char spi_transfer(volatile uint8_t data)
{
  SPDR = data;                    // Start the transmission

  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {};

  return SPDR;                    // return the received byte
}

byte send_SPI_byte(uint8_t val1)
 {
   uint8_t data_byte;
 
   //digitalWrite(CHIPSELECT,LOW);

   data_byte = spi_transfer(val1); //get data byte

   //digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer
 
   return data_byte;
 }

void set_generator()
{
  //выдает 25 МГц                        
  // Program RESET = 1 to reset registers
  writeRegister(R0, 0b0010010000011110);
  //Program RESET = 0 to remove reset
  writeRegister(R0, 0b0010010000011100);

  writeRegister(R78, 0x0003);
  writeRegister(R75, 0x0BC0);       //channel divider = 384
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
  set_power(5);
  writeRegister(R43, 0x0000);
  writeRegister(R42, 0x0000);
  writeRegister(R41, 0x0000);
  writeRegister(R40, 0x0000);
  writeRegister(R39, 0x9680);
  writeRegister(R38, 0x0098);
  writeRegister(R37, 0x0304);
  writeRegister(R36, 960);                  //N dIVEDER
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
  writeRegister(R0, 0b0010010000011100);            
}

void second_set_freq(uint64_t fout)
{

  if(fout > 19000000000)
  {
    //ошибка, больше 19 Ghz нельзя 
  }

  else if(fout < 19000000000 && fout > 15000000000)   //VCO doubler
  {
    //VCO doubler

   
    writeRegister(R45, 0b1101000011011110);   //переключил выход A на VCO Doubler
    writeRegister(R27, 0b0000000000000011);   //включил VCO2X_EN
  }

   else if(fout <15000000000 && fout >7500000000 )    //VCO
  {
    //VCO
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //включил VCO2X_EN    
  }

else        //<7500
  {
    writeRegister(R46, 0b0000011111111100);   //переключил выход B на Channel Divider
    writeRegister(R45, 0b1100000011011110);   //переключил выход A на Channel Divider
    writeRegister(R31, 0b0100001111101100);   //включил CHDIV

    first_five = fout / 1000000;
    last_six = fout % 1000000;

    chdiv = divider_values[find_chdiv(fout)];
    chdiv_reg = reg_divider[find_chdiv(fout)];

    uint32_t pll_den = 10000000;                              
    int NUM = (last_six * chdiv) / pll_den;
    uint16_t pll_n = (first_five * chdiv) / 10 + NUM;   //10 это частота фазового детектора

    long int num_fractional_part = (last_six * chdiv) - NUM * 1e7 + fractional((((float)first_five * (float)chdiv) / 10.0)) * 1e6;        //дробная часть от NUM. (int) NUM * 1e7 необходимо, чтобы убрать целую часть(если она есть), так как ее я уже прибавил к pll_n

    // tft.setCursor(0,58);                         параметры делителей для установки частоты 
    // tft.setTextColor(ST77XX_YELLOW);  
    // tft.fillRect(0, 58, 320, 21, ST77XX_BLACK);
    // tft.print("pll_n = ");
    // tft.print(pll_n);
    // tft.setCursor(0,83);
    // tft.fillRect(0, 83, 320, 21, ST77XX_BLACK);
    // tft.print("PLL_NUM=");
    // tft.print(num_fractional_part);
    // tft.setCursor(0,108);
    // tft.fillRect(0, 108, 320, 21, ST77XX_BLACK);
    // tft.print("chdiv=");
    // tft.print(chdiv);

    writeRegister(R75, chdiv_reg);   

    writeRegister(R36, pll_n);
    writeRegister(R43, low_16bit(num_fractional_part));
    writeRegister(R42, high_16bit(num_fractional_part));

    writeRegister(R0, 0b0010010000011100);

  }
}

