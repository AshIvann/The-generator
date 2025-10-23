#include <Arduino.h>
#include <math.h>
#include "stdint.h"
#include "Print.h"

#include "LMX2595.h"
#include "Display.h"
#include "Encoder.h"

LMX2595 gen;
Display dis;
My_encoder my_enc;
My_keybord my_key;
Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


uint64_t freq_set_by_encoder = 425000000;
uint8_t power_counter = 5;

bool freq_changed = false;          //нужно что-то придумать. Тк переменная не обращается в false после первого вызова
bool power_changed = false;         //нужно что-то придумать. Тк переменная не обращается в false после первого вызова    

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
    tft.print(power_counter);
    tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

    enc1.setTickMode(TYPE2);
    gen.set_generator(425000000, power_counter); 
}


void loop()
{
    enc1.tick(); 
    char key = keypad.getKey();
    if (key)
    {
        my_key.scan_key(key);                                               
        dis.print_freq(my_key.get_rem(), 100, 30);
        gen.set_freq(my_key.get_rem());
        freq_set_by_encoder = my_key.get_rem();
    }
    if(my_enc.click())              //изменение частоты                                 
    {
        tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
        tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
        my_enc.freq_selection();
        if(freq_changed)
        {
            dis.print_freq(freq_set_by_encoder, 100, 30);  
            gen.set_freq(freq_set_by_encoder);
        }
    }
    else                            //изменение мощности 
    {
        tft.fillRect(8, 106, 265, 2, ST77XX_BLUE);
        tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);
        my_enc.power_selection();
        if(power_changed)
        {
            uint8_t best_pow_level = dis.get_best_level(power_counter, freq_set_by_encoder);
            dis.power_print(dis.find_power_level(power_counter, freq_set_by_encoder));
/*
            tft.setCursor(0, 150);                                                                          //контроль устанавливаемых параметров, потом убрать
            tft.fillRect(0, 150, SCREEN_WIDTH, 20, ST77XX_BLACK);
            tft.print("pwr_count = ");
            tft.print(power_counter);                                           

            tft.setCursor(0, 190);                                                                          //контроль устанавливаемых параметров, потом убрать
            tft.fillRect(0, 190, SCREEN_WIDTH, 20, ST77XX_BLACK);
            tft.print("set_pwr = ");
            tft.print(best_pow_level);
*/
            gen.set_out_power(best_pow_level);
        }
    }       
}