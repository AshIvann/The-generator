
#include <Arduino.h>
#include <math.h>
#include "stdint.h"
#include "Print.h"

#include "Encoder_Keypad.h"                 //нужно убрать из этого файла то, что осталось, сейчас получаю ошибку
// #include "LMX2595.h"
// #include "Display.h"
#include "Encoder.h"

// uint64_t freq = 425000000;          //частота которая вызывается в setup


// uint64_t freq_set_by_encoder = freq;               
// uint64_t freq_set_by_key;  


// uint32_t freq_increment = 1;  

//  uint8_t power_counter = 5;
// uint8_t power_increment = 1;             если перестанет измняться мощность, то раскомментировать       


LMX2595 gen;
// Display dis;
My_encoder my_enc;
My_keybord my_key;
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() 
{
    tft.init(240, 320);
    tft.fillScreen(ST77XX_BLACK);   
    tft.setTextColor(ST77XX_WHITE);
    tft.setRotation(1);
    tft.setTextSize(3);
   
    tft.fillRect(165, 31, 20, 3, ST77XX_BLUE);
    tft.setCursor(10, 30);
                                      
    tft.print("Freq: ");
    tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
    // dis.print_freq(my_enc.freq_set_by_encoder, 100, 30);
    
    tft.setCursor(10, 85);                          
    tft.print("Power: ");
    tft.print(my_enc.power_counter);
    tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

    enc1.setTickMode(TYPE2);
    gen.set_generator(425000000, my_enc.power_counter); 
}

void loop()
{
    enc1.tick(); 
    char key = keypad.getKey();
    if (key)
    {
        my_key.scan_key(key);                                               
        my_enc.freq_set_by_encoder = my_key.get_rem();
    }
    if(my_enc.click())              //изменение частоты                                 
    {
        tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
        tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
        my_enc.update_freq();
    }
    else                            //изменение мощности 
    {
        tft.fillRect(8, 106, 265, 2, ST77XX_BLUE);
        tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);
        my_enc.update_power();
    }
}