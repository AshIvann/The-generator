#include <Arduino.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <math.h>
#include "stdint.h"
#include "GyverEncoder.h"
#include "Keypad.h"

#include "func_for_generator.h"
#include "support_func.h"

uint8_t power_counter = 5;
uint64_t click_counter = 1;


uint16_t freq = 75;          //частота которая вызывается в setup

uint32_t increase_value = 1;     //переменная отвечающая за изменение значения частоты 
uint8_t power_increment = 1;         //переменная отвечающая за изменение значения мощности 

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
    tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
                                      
    tft.print("Freq: ");
    tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);

    tft.setCursor(10, 85);                          
    tft.print("Power: ");
    tft.print(power_counter);
    tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

    print_freq(freq_set_by_encoder, 100, 30);

    enc1.setTickMode(TYPE2);
    set_generator();                                          
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
            tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);       
            tft.setCursor(0, 150);
            tft.print((uint32_t)freq_set_by_key);
        }

        else if(key == 'A')
        {
            tft.setCursor(100,30);
            tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
            if(freq_set_by_key < 12e6)
            {
                freq_set_by_key = 12e6;
                tft.print("12.000000MHz");
                second_set_freq(freq_set_by_key);
            }
            else if(freq_set_by_key >= 19e9)
            {
                freq_set_by_key = 19e9;
                tft.print("19000.000000 MHz");
                second_set_freq(freq_set_by_key);
            }
            else
            {
                print_freq(freq_set_by_key, 100, 30);
                //set_power(find_power_level(number, power));
                second_set_freq(freq_set_by_key);
            }
            freq_set_by_encoder = freq_set_by_key;
            freq_set_by_key = 0;             
        }
    
        else if(key == 'D')
        {
            freq_set_by_key = freq_set_by_key / 10;
            tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);
            tft.setCursor(0, 150);
            tft.print((uint32_t)freq_set_by_key);
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
      //tft.setTextSize(2);
      tft.print("Freq: ");
      tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
    }
    else
    {
      tft.setCursor(10, 85);
      tft.fillRect(8, 106, 265, 2, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      //tft.setTextSize(2);
      tft.print("Power: ");
      tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);
    }
  }    
    
    if(click_counter % 2 == 1)    //изменение частоты 
    {
        if(enc1.isRight())
        {
            freq_set_by_encoder += increase_value;
            if(freq_set_by_encoder >= 19e9) 
            {
                freq_set_by_encoder = 19e9;
            }
            print_freq(freq_set_by_encoder, 100, 30);
            second_set_freq(freq_set_by_encoder);
        }

        if(enc1.isLeft())
        {
            freq_set_by_encoder -= increase_value;
            if(freq_set_by_encoder <= 12e6 )
            {
                freq_set_by_encoder = 12e6;
            }
            print_freq(freq_set_by_encoder, 100, 30);
            second_set_freq(freq_set_by_encoder);
        } 
    }
  
    else                    //изменение мощности 
    {
        if(enc1.isRight())       //увеличение на 1
        {
        power_counter += power_increment;
        if(power_counter >= 11)
        {
            power_counter = 11;                                                                   
        }
        power_print(power_counter);
        set_out_power(find_power_level(freq_set_by_encoder, power_counter));//, st_right_freq, st_left_freq ));
        }

        else if(enc1.isLeft())   //уменьшение на 1
        {
        power_counter -= power_increment;
        if(power_counter <= 0 ) 
        { 
            power_counter = 0;
        }
        power_print(power_counter);
        set_out_power(find_power_level(freq_set_by_encoder, power_counter));//, st_right_freq, st_left_freq ));
        }
    }
  }



