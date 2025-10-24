#include <Arduino.h>
#include <math.h>
#include "stdint.h"
#include "Print.h"

#include "LMX2595.h"
#include "Display.h"
#include "Encoder_keypad.h"

LMX2595 gen;
Display dis;

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
My_keybord my_key;

bool is_freq_changed = false;
bool is_power_changed = false;
bool click_changed = true;
uint8_t power_value = 5;
uint64_t freq_value = 425000000;

uint64_t full_number;
void setup()
{
    dis.set_display();
    dis.print_freq(freq_value);
    dis.power_print(power_value);

    enc1.setTickMode(TYPE2);
    gen.set_generator(freq_value, power_value); 
}

void loop()
{
    enc1.tick(); 
    char key = keypad.getKey();

    if(key >= 0 && key <= 9)
    {
        full_number = my_key.build_number(key);
        dis.my_print(full_number, 50, SCREEN_HEIGHT);
    }
    else if(key == 'A')
    {
        freq_value = full_number;
        my_key.entered_number = 0;
        if(freq_value < 12e6)
        {
            freq_value = 12e6;
        }
        else if(freq_value >= 19e9)
        {
            freq_value = 19e9;
        }
        dis.print_freq(freq_value);
        gen.set_freq(freq_value);
    }
    else if(key == 'B')
    {
        freq_value = full_number * 10000000;
        my_key.entered_number = 0;
        if(freq_value < 12e6)
        {
            freq_value = 12e6;
        }
        else if(freq_value >= 19e9)
        {
            freq_value = 19e9;
        }
        dis.print_freq(freq_value);
        gen.set_freq(freq_value);
    }
    else if(key == 'D')
    {
        full_number = my_key.delete_digit();
    }

    if(my_key.isClick())
    {
        click_changed = !click_changed;
    }
    if(click_changed)                                           //изменение частоты                                 
    {
        tft.fillRect(8, SCREEN_HEIGHT/3, 240, 2, ST77XX_BLUE);
        tft.fillRect(8, SCREEN_HEIGHT/2, SCREEN_WIDTH, 2, ST77XX_BLACK);
        if(my_key.isRight())
        {
            gen.freq_increas();
            if(freq_value >= 19e9) 
            {
                freq_value = 19e9;
            }
            is_freq_changed = !is_freq_changed;
        }
        if(my_key.isLeft())
        {
            gen.freq_decreas();
            if(freq_value <= 12e6) 
            {
                freq_value = 12e6;
            }
            is_freq_changed = !is_freq_changed;
        }
        if(is_freq_changed)
        {
            dis.print_freq(freq_value);  
            gen.set_freq(freq_value);
            is_freq_changed = !is_freq_changed;
        }
    }
    else                                                        //изменение мощности 
    {
        tft.fillRect(8, SCREEN_HEIGHT/2, 265, 2, ST77XX_BLUE);
        tft.fillRect(8, SCREEN_HEIGHT/3, SCREEN_WIDTH, 2, ST77XX_BLACK);
        if(my_key.isRight())
        {
            gen.power_increas();
            if(power_value >= 11) 
            {
                power_value = 11;
            }
            is_power_changed = !is_power_changed;
        }
        if(my_key.isLeft())
        {
            gen.power_decreas();
            if(power_value == 255) 
            {
                power_value = 0;
            }
            
            is_power_changed = !is_power_changed;
        }
        if(is_power_changed)
        {
            uint8_t best_pow_level = gen.get_best_level(power_value, freq_value);
            dis.power_print(gen.find_power_level(power_value, freq_value));
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
            is_power_changed = !is_power_changed;
        }
    }       
}