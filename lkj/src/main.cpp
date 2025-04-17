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


 #define CS_PIN 7
 #define SCLK_PIN 13
 #define SDI_PIN 11  // SPI MOSI

/*
 //для LMX2595
    #define MOSI 7
    #define CS 6
    #define SCK 5

    #define R0 0x00
    #define R1 0x01
    #define R7 0x07
*/



Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


 void testdrawtext(char *text, uint16_t color) ;
 void writeRegister();


void setup(){
    tft.init(240, 320);
    tft.fillScreen((uint16_t)(-1));
}




uint16_t cnt = 10;
uint16_t cnt_x = 0;
int x= 0;
int y = 0;
void loop() {
    //tft.fillScreen(ST77XX_BLACK);
    //tft.setCursor(x, y);

    uint32_t color[9] = {
        0x07E0,
        0x001F,
        0xF81F,
        0xFC00,
        0xF800,
        0x0000, 
        0x07FF, 
    };
    for(int i = 0; i< 100; i++){
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
 



  




/*
for (int16_t y=0; y < tft.height(); y+=5) {
  tft.drawFastHLine(0, y, tft.width(), ST77XX_WHITE);
}
for (int16_t x=0; x < tft.width(); x+=5) {
  tft.drawFastVLine(x, 0, tft.height(), ST77XX_BLUE);
}
*/



}

void testdrawtext(char *text, uint16_t color) 
{ 
    tft.setCursor(0, 0);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.print(text);
}
void writeRegister(uint8_t address, uint16_t data)
{
  struct st_packet
  {
    uint8_t rw : 1;
    uint8_t addr : 7;
    uint16_t data;
    /* data */
  };
  st_packet packet;
  packet.rw = 0;
  packet.addr = address;
  packet.data = data;
  digitalWrite(CS_PIN, LOW);
  SPI.transfer((uint8_t)(&packet));// Старший байт
  SPI.transfer((uint8_t)(&packet + 1));
  SPI.transfer((uint8_t)(&packet + 2));// Младший байт
  digitalWrite(CS_PIN, HIGH);

  delay(10); 
}

// put function definitions here:
