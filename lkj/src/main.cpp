#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
 #include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
 #include <SPI.h>



// put function declarations here:
 #define TFT_CS   10     //chip select
 #define TFT_DC   8     //datacomand 
 #define TFT_RST  9     //reset
 //mosi 11
 //sck 13


 /*
 #define CS_PIN 7     
 #define SCLK_PIN 13                            Вроде как ненужно
 #define SDI_PIN 11  // SPI MOSI
*/

 //для LMX2595
  #define MOSI 7          //
  #define CS 6            //////
  #define SCK 5       ////





  
  
  
  #define R0 0x00
  #define R1 0x01
  #define R7 0x07
  #define R8 0x08
  #define R9 0x09
  #define R10 0x0A
  #define R11 0x0B
  #define R12 0x0C
  #define R14 0x0E

  #define R16 0x10   ///
  #define R17 0x11
  #define R19 0x13
  #define R20 0x14
  #define R27 0x1B
  #define R31 0x1F
  #define R34 0x22
  #define R36 0x24
  #define R37 0x25   /////
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
  
  
  
  
  
  
  




Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);



 void testdrawtext(char *text, uint16_t color) ;
//  void writeRegister();
 void writeRegister(uint8_t address, uint16_t data);


void setup() 
{
  
 pinMode(CS, OUTPUT);
 digitalWrite(CS, HIGH);
 SPI.begin();
 SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); // 4 МГц, Mode 0
  //Serial.begin(9600);

  tft.init(240, 320);
  tft.fillScreen((uint16_t)(-1));


// Program RESET = 1 to reset registers
  writeRegister(R0, 0b0010010000011110);

  //Program RESET = 0 to remove reset
  writeRegister(R0, 0b0010010000011100);

  writeRegister(R78, 0b0000000000000011);
  writeRegister(R75, 0b0000100001000000);
  writeRegister(R74, 0b0000000000000000);
  writeRegister(R73, 0b0000000000111111); //разобраться с  SYSREF, вообще что это и для чего
  writeRegister(R72, 0b0000000000000001); //разобраться с  SYSREF, вообще что это и для чего 
  writeRegister(R71, 0b0000000010000001); //Проверить все, разобраться с  SYSREF_--- и тому подобным)
  

    
  writeRegister(R70, 0b1100001101010000);   //можно заменить на 0, влияет тольео на задержку, тк VCO_PHASE_SYNC = 0
  writeRegister(R69, 0b0000000000000000);
  writeRegister(R60, 0b0000000000000000);
  writeRegister(R59, 0b0000000000000001);
  writeRegister(R46, 0b0000011111111101);
  writeRegister(R45, 0b1100100011011111);    //тут сделал не так как в коде, который в блакноте, R45[12:11] = 01
  writeRegister(R44, 0b0001111110100011);    //Output B powered down,Output A Active,OUTA_PWR = 
  writeRegister(R43, 0b0000000000000000);
  writeRegister(R42, 0b0000000000000000);
  writeRegister(R41, 0b0000000000000000);    //MASH_SEED тк в R37 0 поставил в D15
  writeRegister(R40, 0b0000000000000000);    //MASH_SEED тк в R37 0 поставил в D15
  writeRegister(R39, 0b0000001111101000);    //PLL_DEN
  writeRegister(R38, 0b0000000000000000);       //PLL_DEN
  writeRegister(R37, 0b0000010000000100);  //разобраться сMASH _SEE D_EN и настройка PFD_DLY_SEL, которая зависит от частоты VCO и N
  writeRegister(R36, 0b0000000111110100);
  writeRegister(R34, 0b0000000000000000);
  writeRegister(R31, 0b0000001111101100);         //CHDIV_DIV2 = 0, разобраться зачем нужен  
  writeRegister(R27, 0b0000000000000010);
  writeRegister(R20, 0b1111100001001000);        //Не понятно зачем VCO_SEL_STRT_EN, разобраться
  writeRegister(R19, 0b0010011110110111);         //Не понятно зачем, разобраться 
  writeRegister(R17, 0b0000000100101100);
  writeRegister(R16, 0b0000000010000000); // Возможно не нужно, тк VCO_DACISET_FORCE = 0
  writeRegister(R14, 0b0001111001110000);   //charge-pump current = 15mA
  writeRegister(R12, 0b0101000000000001);
  writeRegister(R11, 0b0000000000011000);
  writeRegister(R10, 0b0001000011011000);
  writeRegister(R9, 0b0001011000000100);
  writeRegister(R8, 0b0010000000000000);
  writeRegister(R7, 0b0100000010110010);
  writeRegister(R1, 0b0000100000001000);
  writeRegister(R0, 0b0010010000011100); 
  

}


void loop() {
  //tft.fillScreen(ST77XX_BLACK);
  //tft.setCursor(x, y);
  
  /*
  uint16_t cnt = 10;
  uint16_t cnt_x = 0;
  int x= 0;
  int y = 0;

  uint32_t color[9] = 
  {
    0x07E0,
    0x001F,
    0xF81F,
    0xFC00,
    0xF800,
    0x0000, 
    0x07FF, 
  };
  
for(int i = 0; i< 50; i++)
{ 
 tft.setRotation(0);
 
 tft.setTextSize(2);
 tft.setTextColor(color[i % 7]);
 tft.print(i);
 tft.setTextColor(0x0000);                                 //    вывод чисел 
 tft.print(";");
 
 if(i % 10 >9 )
 tft.setCursor(0, y + 5);
 delay(100); 
}
 
*/

  


  // Program RESET = 1 to reset registers
                                                  // writeRegister(R0, 0000000000000010); 
 //writeRegister(R0, 0b0010010000011110);  
  //Program RESET = 0 to remove reset
                                                // writeRegister(R0, 0000000000000000); 
 //writeRegister(R0, 0b0010010000011100);


  //Начало, то что выше всегда нужно перед началом работы 

  // writeRegister(R78, 0b0000000000000011);
  // writeRegister(R75, 0b0000100001000000);
  // writeRegister(R74, 0b0000000000000000);
  // writeRegister(R73, 0b0000000000111111); //разобраться с  SYSREF, вообще что это и для чего
  // writeRegister(R72, 0b0000000000000001); //разобраться с  SYSREF, вообще что это и для чего 
  // writeRegister(R71, 0b0000000010000001); //Проверить все, разобраться с  SYSREF_--- и тому подобным)
  
  
  // writeRegister(R70, 0b1100001101010000);   //можно заменить на 0, влияет тольео на задержку, тк VCO_PHASE_SYNC = 0
  // writeRegister(R69, 0b0000000000000000);
  // writeRegister(R60, 0b0000000000000000);
  // writeRegister(R59, 0b0000000000000001);
  // writeRegister(R46, 0b0000011111111101);
  // writeRegister(R45, 0b1100100011011111);    //тут сделал не так как в коде, который в блакноте, R45[12:11] = 01
  // writeRegister(R44, 0b0001111110100011);    //Output B powered down,Output A Active,OUTA_PWR = 
  // writeRegister(R43, 0b0000000000000000);
  // writeRegister(R42, 0b0000000000000000);
  // writeRegister(R41, 0b0000000000000000);    //MASH_SEED тк в R37 0 поставил в D15
  // writeRegister(R40, 0b0000000000000000);    //MASH_SEED тк в R37 0 поставил в D15
  // writeRegister(R39, 0b0000001111101000);    //PLL_DEN
  // writeRegister(R38, 0b0000000000000000);       //PLL_DEN
  // writeRegister(R37, 0b0000010000000100);  //разобраться сMASH _SEE D_EN и настройка PFD_DLY_SEL, которая зависит от частоты VCO и N
  // writeRegister(R36, 0b0000000111110100);
  // writeRegister(R34, 0b0000000000000000);
  // writeRegister(R31, 0b0000001111101100);         //CHDIV_DIV2 = 0, разобраться зачем нужен  
  // writeRegister(R27, 0b0000000000000010);
  // writeRegister(R20, 0b1111100001001000);        //Не понятно зачем VCO_SEL_STRT_EN, разобраться
  // writeRegister(R19, 0b0010011110110111);         //Не понятно зачем, разобраться 
  // writeRegister(R17, 0b0000000100101100);
  // writeRegister(R16, 0b0000000010000000); // Возможно не нужно, тк VCO_DACISET_FORCE = 0
  // writeRegister(R14, 0b0001111001110000);   //charge-pump current = 15mA
  // writeRegister(R12, 0b0101000000000001);
  // writeRegister(R11, 0b0000000000011000);
  // writeRegister(R10, 0b0001000011011000);
  // writeRegister(R9, 0b0001011000000100);
  // writeRegister(R8, 0b0010000000000000);
  // writeRegister(R7, 0b0100000010110010);
  // writeRegister(R1, 0b0000100000001000);
  // writeRegister(R0, 0b0010010000011100); 














  /*
  //опорная частота 
  writeRegister(R1, 0b0000100000001000);      //CAL_CLK_DIV--> f_osc<200MHz  
  writeRegister(R9, 0b0001011000000100);      // R9[12]=1, Умножаем опорный сигнал на 2 
  writeRegister(R12, 0b0101000000000001);     // Делим PLL_R_PRE = 1
  writeRegister(R10, 0b0001000011011000);     //R10[11;7] = 1; умножаем на 1(это MULT)
  writeRegister(R11, 0b0000000000011000);     //R11[11;4] = 1; делим на 1(это PLL_R)
       //charge-pump current = 15mA
  
  writeRegister(R43, 0b0000000000000000);     //настройка числителя = 0
  writeRegister(R38, 0b0000000000000000);     //PLL_DEN
  writeRegister(R39, 0b0000000000000001);     //PLL_DEN 
  
  writeRegister(R34, 0b0000000000000001);  //зачем выделено 3 бита?   // PLL_N = enable
  writeRegister(R36, 0b0000000111110100);     // PLL_N =500

  writeRegister(R44, 0b0000111100000000 );     //OUT_A/B  active, OUTA_PWR[13:8] = 15
  writeRegister(R45, 0b1100100011001111 );     //OUT_B_PWR[5:0] = 15,  OUTA_MUX = 1

*/
  
}


void testdrawtext(char *text, uint16_t color) 
{ 
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

  

void writeRegister(uint8_t addr, uint16_t data) {

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

  digitalWrite(CS, LOW);

  SPI.transfer(bytes[0]);
  SPI.transfer(bytes[2]);
  SPI.transfer(bytes[1]);

  digitalWrite(CS, HIGH);


  /*
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK); 
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Белый текст на черном фоне
  tft.setTextSize(2); 
  tft.setCursor(0, 0); 
  tft.println("Addr:"+ String(addr, BIN)); // Вывод адреса в 2-ричном формате
  tft.println("Data:" + String(data, BIN));
*/

  delay(10);
}