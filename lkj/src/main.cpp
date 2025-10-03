#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <math.h>
#include "stdint.h"
#include "GyverEncoder.h"
#include "Keypad.h"
#include "Print.h"

#include "func_for_generator.h"
#include "support_func.h"


uint8_t power_counter = 5;
uint64_t click_counter = 1;

uint64_t freq = 75;          //частота которая вызывается в setup
uint64_t power = 5;

uint32_t freq_increment = 1;      
uint8_t power_increment = 1;         

uint64_t freq_set_by_key;  

 
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

    tft.setCursor(10, 85);                          
    tft.print("Power: ");
    tft.print(power_counter);
    tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

    print_freq(freq_set_by_encoder, 100, 30);

    enc1.setTickMode(TYPE2);
    set_generator(freq, power);                                          
}

void loop()
{
    enc1.tick(); 
    char key = keypad.getKey();

    if (key)
    {
        if(key >= '0' && key <= '9')
        {
            freq_set_by_key = (freq_set_by_key * 10) + (key - 48);
            print_freq(freq_set_by_key, 0, 150);                                       //Изменил строку, проверить что будет выведенно 
        }

        else if(key == 'A')
        {
            tft.setCursor(100,30);
            tft.fillRect(100, 30, SCREEN_WIDTH, 21, ST77XX_BLACK);
            if(freq_set_by_key < 12e6)
            {
                freq_set_by_key = 12e6;
                tft.print("12.000000 MHz");
            }
            else if(freq_set_by_key >= 19e9)
            {
                freq_set_by_key = 19e9;
                tft.print("19000.000000 MHz");
            }
            print_freq(freq_set_by_key, 100, 30);
            set_freq(freq_set_by_key);
            freq_set_by_encoder = freq_set_by_key;
            freq_set_by_key = 0;             
        }
    
        else if(key == 'D')
        {
            freq_set_by_key = freq_set_by_key / 10;
            print_freq(freq_set_by_key, 0, 150);
        }
    }

    if(enc1.isClick())
    {
        click_counter = click_counter + 1;
        if(click_counter % 2 == 1)
        {
            tft.setCursor(10, 30);
            tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
            tft.setTextColor(ST77XX_WHITE);
            tft.print("Freq: ");
            tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
        }
        else
        {
            tft.setCursor(10, 85);
            tft.fillRect(8, 106, 265, 2, ST77XX_BLUE);
            tft.setTextColor(ST77XX_WHITE);
            tft.print("Power: ");
            tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);
        }
    }    
    
    if(click_counter % 2 == 1)      //изменение частоты 
    {
        if(enc1.isRight())
        {
            freq_set_by_encoder += freq_increment;
            if(freq_set_by_encoder >= 19e9) 
            {
                freq_set_by_encoder = 19e9;
            }
            print_freq(freq_set_by_encoder, 100, 30);
            set_freq(freq_set_by_encoder);
        }

        if(enc1.isLeft())
        {
            freq_set_by_encoder -= freq_increment;
            if(freq_set_by_encoder <= 12e6 )
            {
                freq_set_by_encoder = 12e6;
            }
            print_freq(freq_set_by_encoder, 100, 30);
            set_freq(freq_set_by_encoder);
        }
    }
  
    else                            //изменение мощности 
    {
        if(enc1.isRight())       //увеличение на 1
        {
            power_counter += power_increment;
            if(power_counter >= 11)
            {
                power_counter = 11;                                                                   
            }
            power_print(find_power_level(power_counter, freq_set_by_encoder));
        }

        else if(enc1.isLeft())   //уменьшение на 1
        {
            power_counter -= power_increment;
            if(power_counter <= 0 ) 
            { 
                power_counter = 0;
            }
            power_print(find_power_level(power_counter, freq_set_by_encoder));
        }
    }
}