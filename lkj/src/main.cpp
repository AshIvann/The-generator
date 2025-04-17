#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
 #include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
 #include <SPI.h>



// put function declarations here:
 #define TFT_CS   10
 #define TFT_DC   8
 #define TFT_RST  9

 Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


 void testdrawtext(char *text, uint16_t color) ;
 
void setup() {
  tft.init(240, 320);
  tft.fillScreen((uint16_t)(-1));
}




uint16_t cnt = 0;
uint16_t cnt_x = 0;
int x= 0;
int y = 0;
void loop() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(x, y);
  
   uint32_t color[9] = {
    0x0000, 
    0x07FF, 
    0xF81F,
    0xFC00,
    0xF800,
    0x07E0,
    0x001F
  };
  for(int i = 0; i< 100; i++)
  { 
   
    
    tft.setRotation(0);
    
    tft.setTextSize(2);
    tft.setTextColor(color[i % 7 ]);
    tft.print(i);
    tft.setTextColor(0x0000);
    tft.print(";");
    
    if(i % 10 >9 )
    tft.setCursor(0, y + 5);
    delay(100); 
  }
 
 


  cnt += 1;
  
  delay(100);
  if(cnt == 100) cnt_x++; 
  //tft.writeLine(cnt_x, cnt_x, cnt, cnt, cnt);
  // put your main code here, to run repeatedly:
}

void testdrawtext(char *text, uint16_t color) 
{ 
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
// put function definitions here:
