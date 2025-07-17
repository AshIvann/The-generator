#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <math.h>
#include "GyverEncoder.h"




// мои пины для дислпея 

#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select

//Для энкодера 
#define CLK 2
#define DT 3
#define SW 4

 //Фрагмент из кода для SPI используются для передачи данных на LMX2595
 #define DATAOUT 11//MOSI     
 #define DATAIN  12//MISO     
 #define SPICLOCK  13//sck    //
 #define CHIPSELECT 10//ss    
 



 #define R0 0x00
 #define R1 0x01
 #define R7 0x07
 #define R8 0x08
 #define R9 0x09
 #define R10 0x0A
 #define R11 0x0B
 #define R12 0x0C
 #define R14 0x0E
 #define R16 0x10
 #define R17 0x11
 #define R19 0x13
 #define R20 0x14
 #define R27 0x1B
 #define R31 0x1F
 #define R34 0x22
 #define R36 0x24
 #define R37 0x25
 #define R38 0x26
 #define R39 0x27
 #define R40 0x28
 #define R41 0x29
 #define R42 0x2A
 #define R43 0x2B
 #define R44 0x2C 
 #define R45 0x2D
 #define R46 0x2E
 #define R59 0x3B
 #define R60 0x3C
 #define R69 0x45
 #define R70 0x46
 #define R71 0x47
 #define R72 0x48
 #define R73 0x49
 #define R74 0x4A
 #define R75 0x4B
 #define R78 0x4E
 #define R79 0x4F
 #define R80 0x50
 #define R81 0x51
 #define R82 0x52
 #define R83 0x53
 #define R84 0x54
 #define R85 0x55
 #define R86 0x56
 #define R96 0x60
 #define R97 0x61
 #define R98 0x62
 #define R99 0x63
 #define R100 0x64
 #define R101 0x65
 #define R102 0x66
 #define R103 0x67
 #define R104 0x68
 #define R105 0x69
 #define R106 0x6A
 #define R110 0x6E
 #define R111 0x6F
 #define R112 0x70
 


 

 Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
 //Encoder enc1(CLK, DT, SW);
 Encoder enc1(CLK, DT, SW, TYPE2);

 void set_generator();
 void testdrawtext(char *text, uint16_t color) ;          
 void writeRegister(uint8_t address, uint16_t data);                      //передача дынных в регистр 
 char spi_transfer(volatile uint8_t data);                                //настройка SPI
 void settings_spi();                                                     //настройка SPI
 byte send_SPI_byte(uint8_t val1);                                        //передача байта по SPI
 uint16_t dec_to_bin(uint16_t num);                                       //перевод из десятичного числа в двоичное 
 void set_freq(uint16_t fout, uint16_t power);                                            //установка частоты 
 
 uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits);      //перемещение 6 битов на место 8-13(используется для установки мощности), original изначальный адресс, в котором изменяется 8-13 байт, new_bits биты которые ставятся в 8-13 биты 
 uint8_t fractional(float number);                                        //отделяет цифры после запятой, возможно пригодится 
 bool isInteger(float number);                                            //проверяет является ли число дробным или нет. Елс целое то возвращает 1, если нет то 0

 byte clr;
 uint8_t address=0;
 
 
 

const int cs = 10; //для проверки 

int counter = 75;
int power_counter = 10;
int click_counter = 1;
int increment = 1;

uint16_t freq = 75;          //частота которая вызывается в setup
uint16_t power = 10;

void setup() 
  {
    //settings_spi();
    
    tft.init(240, 320);
    
    tft.fillScreen(ST77XX_BLACK); 
    tft.setRotation(0);   

    // Set text color and size
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
  
    // Display static labels
    tft.setCursor(10, 20);
    tft.print("Freq: ");
    tft.print(freq);
    tft.fillRect(8, 40, 160, 2, ST77XX_BLUE);
    tft.print(" MHz");
    tft.setCursor(10, 60);
    tft.print("Power: ");
    tft.print(power);
    tft.print(" ???");

    Serial.begin(9600);

    enc1.setTickMode(TYPE2);


    set_generator;


  /*
    // //   ramp_mode
    // writeRegister(R112, 0x00);
    // writeRegister(R111, 0x00);
    // writeRegister(R110, 0x00);
    // writeRegister(R106, 0x00);
    // writeRegister(R105, 0x0000);
    // writeRegister(R104, 0x346E);
    // writeRegister(R103, 0xF972);
    // writeRegister(R102, 0x3FFF);
    // writeRegister(R101, 0x0010);
    // writeRegister(R100, 0x346E);
    // writeRegister(R99, 0x346E);
    // writeRegister(R98, 0x0000);
    // writeRegister(R97, 0x8800);
    // writeRegister(R96, 0x0000);
    // writeRegister(R86, 0x0002);
    // writeRegister(R85, 0x0000);
    // writeRegister(R84, 0x0000);
    // writeRegister(R83, 0x0005);
    // writeRegister(R82, 0x0000);
    // writeRegister(R81, 0x0000);
    // writeRegister(R80, 0x0000);
    // writeRegister(R79, 0x0080);
  
    // writeRegister(R0, 0b1010010000011100);

   */
}
     

  uint16_t cnt = 10;
  uint16_t cnt_x = 0;
  int x= 0;
  int y = 0;

void loop()
{
  enc1.tick();
  if(enc1.isClick())
  { 
    click_counter = click_counter + 1;
    if(click_counter % 2 == 1)
    {
      tft.setCursor(10, 20);
      tft.fillRect(8, 40, 160, 2, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.print("Freq: ");
      tft.fillRect(8, 80, 160, 2, ST77XX_BLACK);
      
    }
    else
    {
      tft.setCursor(10, 60);
      tft.fillRect(8, 80, 160, 2, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.print("Power: ");
      tft.fillRect(8, 40, 160, 2, ST77XX_BLACK);
    }
      
  }

  int increment = 1;
  int power_increment = 1;


  if(enc1.isFastL())
  {
    tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(80, 20);
      tft.fillRect(80, 20, 100, 20, ST77XX_BLACK); 
      tft.print("fast_L");
  }

  
  if(enc1.isFastR())
  {
    tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(80, 20);
      tft.fillRect(80, 20, 100, 20, ST77XX_BLACK); 
      tft.print("fast_R");
  }


  /*       //выбор параметра при нажатии, изменение при повороте только на 1 
  if(click_counter % 2 == 1)
  {
 
    if(enc1.isRight()) 
    {
      counter += increment;
      ///tft.setRotation(0);                                            //фрагмент для энкодера
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(80, 20);
      tft.fillRect(80, 20, 100, 20, ST77XX_BLACK); 
      if(counter < 10)
      {
        tft.print("error");
      }
      else
      {
        tft.print(counter);
        freq = counter;
        tft.print(" MHz");
        set_freq(freq, power);
      }
    }
   
      else if (enc1.isLeft()) 
      {
        counter -= increment ;
        //tft.setRotation(0);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(80, 20);
        tft.fillRect(80, 20, 100, 20, ST77XX_BLACK); 
        if(counter< 10)
        {
          tft.print("error");
        }
        else
        {
          tft.print(counter);
          freq = counter;
          tft.print(" MHz");
          set_freq(freq, power);
        }
      }
  } 

  else
  {
    if (enc1.isRight()) 
    {
      power_counter += power_increment;
      //tft.setRotation(0);                                            
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(90, 60);
      tft.fillRect(80, 60, 40, 20, ST77XX_BLACK); 
      if(power_counter < 0 || power_counter >= 30 )
      {
        tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
        power_counter = 30;
        tft.print(power_counter);
      }
      else
      {
        tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
        tft.print(power_counter);
        power = power_counter;
        tft.print(" ???");
        set_freq(freq, power);
      }
    }
   
      else if (enc1.isLeft()) 
      {
        power_counter -= power_increment;
        //tft.setRotation(0);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(90, 60);
        tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
        
        if(power_counter > 30 )
        {
          tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
          power_counter = 30;
          tft.print(power_counter);
        }
        else if(power_counter < 0 )
        {
          tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
          power_counter = 0;
          tft.print(power_counter);
        }
        else
        {
          tft.fillRect(80, 60, 100, 20, ST77XX_BLACK); 
          tft.print(power_counter);
          power = power_counter;
          tft.print(" ???");
          set_freq(freq, power);
        }
      }
  }
*/
  


  /*попытка использовать ramp mod

  // Program RESET = 1 to reset registers
  writeRegister(R0, 0b0010010000011110);

  //Program RESET = 0 to remove reset
  writeRegister(R0, 0b0010010000011100);




  writeRegister(R106, 0b0000000000000000);   // не знаю что такое  RAMP_SCALE_COUNT, пусть равен 0

  //1 ПОСТАВИЛ ПРОСТО ТАК, ЧТОБЫ БЫЛО КАКОЕТО ЧИСЛО, А НЕ 0 В  RAMP_DLY_CNT
  //Возможно вместо него нужно использовать R98, где есть RAMP0_DLY
  writeRegister(R105, 0b0000000000000000);    

  writeRegister(R104, 0b0011010001101110);     // RAMP_LEN1 = 13422
  writeRegister(R100, 0b0011010001101110);     // RAMP_LEN0 = 13422

  writeRegister(R101, 0b0000000000010001);  //тут  не понятно что делать с  RAMP1_DLY(пока что 0)

  writeRegister(R103, 0b1111100101110010);  //RAMP1_INC = 1 073 740 146
  writeRegister(R102, 0b0011111111111111);  //RAMP1_INC = 1 073 740 146

  //Возможно нужно будет поменять местами, тк я не знаю с какого регистра начинается запись 
  writeRegister(R99, 0b1111100101110010);     // RAMP0_INC = 1 073 740 146, как для RAMP1_INC, хотя в документации написано другое 
  writeRegister(R98, 0b1111111111111100);     // RAMP0_INC = 1 073 740 146, как для RAMP1_INC, хотя в документации написано другое 


  writeRegister(R97, 0b1000100000000000);   //RAMP0_RST,RAMP_TRIGB, RAMP_TRIGA, RAMP_BURST_TRIG
  writeRegister(R96, 0b1000000000000000);   // не знаю что ставить в RAMP_BURST_EN, пока что 1 



  writeRegister(R80, 0b0000000000000011);      //RAMP_THRESH, почему 11 бит отдельно?
  writeRegister(R79, 0b0000000010000000);      //RAMP_THRESH, нужно рахобраться какой регистр нужно заполнять первым 
  writeRegister(R78, 0b0000000000000000);      //RAMP_THRESH

  writeRegister(R0, 0b1010010000011100);      //Enable frequency ramping mode
*/

}



void set_generator()
{
  //выдвет 75 МГц                        
    // Program RESET = 1 to reset registers
    writeRegister(R0, 0b0010010000011110);
    //Program RESET = 0 to remove reset
    writeRegister(R0, 0b0010010000011100);

    writeRegister(R78, 0x0003);
    writeRegister(R75, 0x0B00);
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
    writeRegister(R44, 0b0001111010100011);   //OUTA_PWR =30
    writeRegister(R43, 0x0000);
    writeRegister(R42, 0x0000);
    writeRegister(R41, 0x0000);
    writeRegister(R40, 0x0000);
    writeRegister(R39, 0x000A);
    writeRegister(R38, 0x0000);
    writeRegister(R37, 0x0304);
    writeRegister(R36, 960);                  //N dIVEDER
    writeRegister(R34, 0x0000);
    writeRegister(R31, 0x43EC);
    writeRegister(R27, 0x0002);
    writeRegister(R20, 0xE048);                                                            //пока что в комент, в дальнейшем нужно для настройки генератора.
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

void testdrawtext(char *text, uint16_t color) 
{ 
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

char spi_transfer(volatile uint8_t data)
{
  SPDR = data;                    // Start the transmission

  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {};

  return SPDR;                    // return the received byte
}

void settings_spi()
{
  pinMode(SPICLOCK,OUTPUT); 
  pinMode(CHIPSELECT,OUTPUT);
  pinMode(DATAOUT,OUTPUT);

  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,controller,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)

  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
}

byte send_SPI_byte(uint8_t val1)
 {
   uint8_t data_byte;
 
   //digitalWrite(CHIPSELECT,LOW);

   data_byte = spi_transfer(val1); //get data byte

   //digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer
 
   return data_byte;
 }

void writeRegister(uint8_t addr, uint16_t data)
{

  struct st_packet {
        
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

  // SPI.transfer(bytes[0]);
  // SPI.transfer(bytes[2]);
  // SPI.transfer(bytes[1]);

  digitalWrite(CHIPSELECT,LOW);
  send_SPI_byte(bytes[0]);
  send_SPI_byte(bytes[2]);
  send_SPI_byte(bytes[1]);
  digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer

 // digitalWrite(CS, HIGH);
}

uint16_t dec_to_bin(uint16_t num)    //Функция для перевода, но она не нужно, но пусть будет 
{
    uint32_t bin = 0;
    //uint32_t k = 1;
    uint32_t position = 1;

    while (num > 0 )
    {
        bin += (num % 2) * position;
        //k *= 10;
        position <<=1;
        num /= 2;
    }

    return bin;
}

bool isInteger(float number) {
  return number == (int)number;
}

void set_freq(uint16_t fout, uint16_t power)
{
  int chdiv;

  if(fout > 20000)
  {
    //ошибка, больше 20 Ghz нельзя 
  }

  else if(fout < 20000 && fout >15000)   //VCO doubler
  {
    //VCO doubler
    writeRegister(R45, 0b1101000011011110);   //переключил выход A на VCO Doubler
    writeRegister(R27, 0b0000000000000011);   //включил VCO2X_EN

    int PLL_N = ((fout * chdiv) / 10);   
    int PLL_NUM;
  
    if(isInteger(PLL_N) == 0)           //исплбзуется для проверки являеся ли PLL_num дробным или нет. Если целое то можно получить частоты без использования дроби
    {
  
        PLL_NUM = fractional(((fout * chdiv) / 10));
    }                                   
    else
    {
       PLL_NUM = 0;
    }
  
    
    
    writeRegister(R36, PLL_N);
    writeRegister(R43, PLL_NUM);
    writeRegister(R39, 10);
    
    writeRegister(R44, replace_bits_8_to_13(0x1EA3, dec_to_bin(power)));
    writeRegister(R0, 0b0010010000011100);
  
  }

  else if(fout <15000 && fout >7500 )    //VCO
  {
    //VCO
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //включил VCO2X_EN  
    
    int PLL_N = ((fout * chdiv) / 10);   
    int PLL_NUM;
  
    if(isInteger(PLL_N) == 0)           //исплбзуется для проверки являеся ли PLL_num дробным или нет. Если целое то можно получить частоты без использования дроби
    {
  
        PLL_NUM = fractional(((fout * chdiv) / 10));
    }                                   
    else
    {
       PLL_NUM = 0;
    }
  
    
    
    writeRegister(R36, PLL_N);
    writeRegister(R43, PLL_NUM);
    writeRegister(R39, 10);
    
    writeRegister(R44, replace_bits_8_to_13(0x1EA3, dec_to_bin(power)));
    writeRegister(R0, 0b0010010000011100);
  
  }

  else if(fout < 7500)//начало диапазона Channel Divider
  {
    writeRegister(R46, 0b0000011111111100);   //переключил выход B на Channel Divider
    //writeRegister(R45, 0b11000 00 011011110);   //переключил выход A на Channel Divider, power boost- max
    writeRegister(R45, 0b1100011011011110); 
    writeRegister(R31, 0b0100001111101100);   //включил CHDIV
    
  
    if(fout < 7500 && fout >= 3750 )
    {
     // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      writeRegister(R31, 0b0000001111101100);   //выключил CHDIV, при 2 можно выключить, при остальных всегда включен
      writeRegister(R75, 0b0000100000000000);   //chdiv  = 2
      chdiv = 2;

    }
  
    else if(fout < 3750 && fout >= 1875 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100001000000);   //chdiv  = 4
      chdiv = 4;
    }
  
    else if(fout <1875 && fout >= 1250 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100010000000);   //chdiv  = 6
      chdiv = 6;
    }
    
    else if(fout < 1250 && fout >= 936 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100011000000);   //chdiv  = 8
       chdiv = 8;
     
    }
  
    else if(fout < 936 && fout >= 625 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100100000000);   //chdiv  = 12
      chdiv = 12;
    }
  
    else if(fout < 625 && fout >= 468 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100101000000);   //chdiv  = 16
      chdiv = 16;
    }
  
    else if(fout < 468 && fout >= 312 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100110000000);   //chdiv  = 24
      chdiv = 24;
    }
  
    else if(fout < 312 && fout >= 234 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100111000000);   //chdiv  = 32
      chdiv = 32;
    }
  
    else if(fout < 234 && fout >= 156 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101000000000);   //chdiv  = 48
      chdiv = 48;
    }
  
    else if(fout < 156 && fout >= 117 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101001000000);   //chdiv  = 64
      chdiv = 64;
    }
  
    else if(fout < 117 && fout >= 104 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101010000000);   //chdiv  = 72
      chdiv = 72;
    }
  
    else if(fout < 104 && fout >= 78 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101011000000);   //chdiv  = 96
      chdiv = 96;
    }
  
    else if(fout < 78 && fout >= 58 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101100000000);   //chdiv  = 128
      chdiv = 128;

    
    }
  
    else if(fout < 58 && fout >= 39 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101101000000);   //chdiv  = 192
      chdiv = 192;
    }

    else if(fout < 39 && fout >= 29 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101110000000);   //chdiv  = 256
      chdiv = 256;
    }

    else if(fout < 29 && fout >= 19 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101111000000);   //chdiv  = 384
      chdiv = 384;
    }
    
    else if(fout < 19 && fout >= 14 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000110000000000);   //chdiv  = 512
      chdiv = 512;
    }
    
    else if(fout < 14 && fout >= 9 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000110001000000);   //chdiv  = 768
      chdiv = 768;
    }
  }

  float PLL_N = ((fout * chdiv) / 10);   
  int PLL_NUM;
  

  if(isInteger(PLL_N) == 1 )           //проверяет является ли число дробным или нет. Если целое то возвращает 1, если нет то 0 
  {
    PLL_NUM = 0;
  }                                   
  else
  {
    PLL_NUM = fractional(((fout * chdiv) / 10));
  }

  
  
  writeRegister(R36, trunc(PLL_N));
  writeRegister(R43, PLL_NUM);
  writeRegister(R39, 10);
  
  writeRegister(R44, replace_bits_8_to_13(0x1EA3, dec_to_bin(power)));
  writeRegister(R0, 0b0010010000011100);

}

uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits) {
  // Маска для очистки битов 8-13: 0b1100000111111111
  uint16_t mask = 0xC0FF;
  // Убедимся, что new_bits содержит только 6 бит
  new_bits &= 0x3F;
  // Сдвигаем новые биты на позицию 8
  uint16_t shifted_bits = (uint16_t)new_bits << 8;
  // Очищаем биты 8-13 и вставляем новые

  uint16_t final = (original & mask) | shifted_bits;

  return final;
}

uint8_t fractional(float number)          //отделяет цифры после запятой, возможно пригодится 
{
  
    int integer_part = (int)floor(number); // Целая часть
    float fractional_part = number - integer_part; // Дробная часть
    int fractional_digits = (int)(fractional_part * 10); // Умножаем на 100, чтобы получить 13

    

    return fractional_digits;
}